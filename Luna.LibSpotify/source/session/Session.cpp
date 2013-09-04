#include <stdint.h>
#include <memory>
#include "Session.h";
#include "../track/Track.h"
#include "../playlist/Playlist.h"
#include "../playlist/PlaylistContainer.h"
#include "AudioFormat.h"


using namespace System;
using namespace System::Collections::Generic;

namespace Luna {
	namespace LibSpotify {

		
		Session::Session(){
			actualTrack = nullptr;
			actionStack = gcnew List<ActionInfo^>();
			state = SessionState::None;
			processLock = gcnew Object();
			processEvent = gcnew AutoResetEvent(false);
			nextEventCall = 0;
			session = nullptr;
			sessionConfig = gcnew LibSpotify::SessionConfig();
			sessionTask = gcnew Task(gcnew Action<Object^>(this, &Session::sessionRunner), gcnew Object(), CancellationToken::None, System::Threading::Tasks::TaskCreationOptions::LongRunning);
			sessionTask->Start(TaskScheduler::Default);
			deliveryBufferSize = initialDeliveryBufferSize;
			deliveryBuffer = gcnew array<short>(deliveryBufferSize);
		}

		Session::~Session(){
			isDisposed = true;

			if (session != nullptr){
				delete session;
			}
		}

		/** threading **/

		void Session::sessionRunner(Object^ state) { 
			bool working = false;
			while(!isDisposed) {

				if (!working) {
					if (sessionTask->CreationOptions == System::Threading::Tasks::TaskCreationOptions::LongRunning) {
						Thread::Sleep(10);
					//	processEvent->WaitOne();
					}
				}
				working = false;

				if (actionStack->Count > 0) {
					for(int n=0; n<actionStack->Count; n++) {
						Monitor::Enter(processLock);
						try{
							ActionInfo^ nextAction = actionStack[n];					
							if ((nextAction->needsValidSession) && (state->Equals(SessionState::None))) {
								continue;
							}

							actionStack->Remove(nextAction);

							if (dynamic_cast<Action^>(nextAction->action) != nullptr) {
								(dynamic_cast<Action^>(nextAction->action))->Invoke();
							} else if (dynamic_cast<Action<Object^>^>(nextAction->action) != nullptr) {
								(dynamic_cast<Action<Object^>^>(nextAction->action))->Invoke(nextAction->arg);
							}
						} finally {
							Monitor::Exit(processLock);
						}

						break;
					}

					working = actionStack->Count > 0;
				}

				if (!state->Equals(SessionState::None)) {
					if ((Environment::TickCount >= nextEventCall) && (session != nullptr)) {
						Monitor::Enter(processLock);
						try{
							int nextCall = 0;
							sp_session_process_events(session, &nextCall);
							nextEventCall = nextCall / 1000;
						} finally {
							Monitor::Exit(processLock);
						}

						working = true;
					}
				}
			}
		}

		/** invoke **/

		void Session::invoke(Action^ action, bool needsActiveSession) {
			ActionInfo^ info = gcnew ActionInfo();
			info->action = action;
			info->arg = nullptr;
			info->needsValidSession = needsActiveSession;

			Monitor::Enter(processLock);
			try{
				actionStack->Add(info);
				processEvent->Set();
			} finally {
				Monitor::Exit(processLock);
			}
		}

		void Session::invoke(Action<Object^>^ action, Object^ arg, bool needsActiveSession) {
			ActionInfo^ info = gcnew ActionInfo();
			info->action = action;
			info->arg = arg;
			info->needsValidSession = needsActiveSession;
			
			Monitor::Enter(processLock);
			try{
				actionStack->Add(info);
				processEvent->Set();
			} finally {
				Monitor::Exit(processLock);
			}
		}

		/** session **/

		void Session::createSession(){
			if (state == SessionState::SessionCreated) {
				throw gcnew Exception("Session is already created");
			}

			invoke(gcnew Action(this, &Session::createSessionInternal), false);
		}

		void Session::createSessionInternal() {
			sp_session* sp;
			sp_error errorCode = sp_session_create(sessionConfig->sessionConfig, &sp);
			session = sp;

			switch(errorCode){
				case sp_error::SP_ERROR_OK:
					sessionId = (int)sp;
					sessionTable->Add(sessionId, this);
					state = SessionState::SessionCreated;
					break;

				default:
					state = SessionState::None;
					sessionId = 0;
					break;
			}
					
			SpErrorCode error = static_cast<SpErrorCode>(errorCode);
			SessionCreated(this, error);
			onSessionCreated(error);
		}

		void Session::login(String^ username, String^ password) {
			if (state < SessionState::LoggedIn) {			
				UserInfo^ userInfo = gcnew UserInfo();
				userInfo->Username = username;
				userInfo->Password = password;

				invoke(gcnew Action<Object^>(this, &Session::loginInternal), userInfo, true);
			} else {
				throw gcnew Exception("Could not log in: Actual user must be logged out before loggin in again.");
			}
		}

		void Session::loginInternal(Object^ arg) {
			if (dynamic_cast<UserInfo^>(arg) != nullptr) {
				UserInfo^ userInfo = dynamic_cast<UserInfo^>(arg);
				const char* user = InteropUtilities::convertToCString(userInfo->Username);
				const char* passwd = InteropUtilities::convertToCString(userInfo->Password);

				sp_error error = sp_session_login(session, user, passwd, false, nullptr);
			}
		}

		void Session::logout(){
			if (state >= SessionState::LoggedIn) {			
				invoke(gcnew Action(this, &Session::logoutInternal), false);
			} else {
				throw gcnew Exception("Could not log out: No user is logged in.");
			}
		}

		void Session::logoutInternal() {
			if (session != nullptr) {
				Monitor::Enter(processLock);
				try{
					sp_user_release(ActiveUser->user);
					sp_error result = sp_session_logout(session);
					sp_connectionstate state = sp_session_connectionstate(session);

					nextEventCall = 0;
				} finally {
					Monitor::Exit(processLock);
				}
			}
		}
		
		void Session::releaseSession(){
			if (state >= SessionState::SessionCreated) {
				invoke(gcnew Action(this, &Session::releaseSessionInternal), false);
			} else {
				throw gcnew Exception("Session could not be released: No session created");
			}
		}

		void Session::releaseSessionInternal() {
			if (session != nullptr){
				Monitor::Enter(processLock);
				try{
					sessionTable->Remove(sessionId);
					sp_session_release(session);
					session = nullptr;

					SessionStopped(this, gcnew EventArgs());
					onSessionReleased();
				} finally {
					state = SessionState::None;
					Monitor::Exit(processLock);
				}
			}
		}

		void Session::forgetme() {
			if (state >= SessionState::SessionCreated) {
				sp_session_forget_me(session);
			} else {
				throw gcnew Exception("Could not call function: No session created");
			}
		}

		void Session::flushCaches() {
			if (state >= SessionState::SessionCreated) {
				sp_session_flush_caches(session);
			} else {
				throw gcnew Exception("Could not call function: No session created");
			}
		}

		void Session::setCacheSize(int size) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_set_cache_size(session, size);

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set cache size: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set cache size: No session created");
			}
		}

		void Session::setPreferredBitrate(Bitrate bitrate) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_preferred_bitrate(session, static_cast<sp_bitrate>(bitrate));

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set preferred bitrate: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set preferred bitrate: No session created");
			}
		}

		void Session::setPreferredOfflineBitrate(Bitrate bitrate, bool resynchronize) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_preferred_offline_bitrate(session, static_cast<sp_bitrate>(bitrate), resynchronize);

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set preferred offline bitrate: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set preferred offline bitrate: No session created");
			}
		}
		
		void Session::setPreferredBitrates(Bitrate onlineBitrate, Bitrate offlineBitrate, bool resynchronize) {
			setPreferredBitrate(onlineBitrate);
			setPreferredOfflineBitrate(offlineBitrate, resynchronize);
		}

		void Session::setScrobbling(SocialProvider provider, ScrobblingState scrobblingState) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_set_scrobbling(session, static_cast<sp_social_provider>(provider), static_cast<sp_scrobbling_state>(scrobblingState));
				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set scrobbling state: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set set scrobbling state: No session created");
			}
		}

		ScrobblingState Session::getScrobblingState(SocialProvider provider) {
			if (state >= SessionState::SessionCreated) {
				sp_scrobbling_state scrobblingState;
				sp_error error = sp_session_is_scrobbling(session, static_cast<sp_social_provider>(provider), &scrobblingState);

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not obtain scrobbling state: {0}", static_cast<SpErrorCode>(error)));
				}

				return static_cast<ScrobblingState>(state);
			} else {
				throw gcnew Exception("Could not set obtain state: No session created");
			}
		}

		bool Session::isScrobblingPossible(SocialProvider provider) {
			if (state >= SessionState::SessionCreated) {
				bool possible;
				sp_error error = sp_session_is_scrobbling_possible(session, static_cast<sp_social_provider>(provider), &possible);

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not obtain scrobbling possibility: {0}", static_cast<SpErrorCode>(error)));
				}

				return possible;
			} else {
				throw gcnew Exception("Could not obtain scrobbling possibility: No session created");
			}
		}

		void Session::setScrobblingCredentials(SocialProvider provider, String^ username, String^ password) {
			if (state >= SessionState::SessionCreated) {
				if ((!String::IsNullOrEmpty(username)) && (!String::IsNullOrEmpty(password))) {
					const char* cstr_user = InteropUtilities::convertToCString(username);
					const char* cstr_password = InteropUtilities::convertToCString(password);

					sp_error error = sp_session_set_social_credentials(session, static_cast<sp_social_provider>(provider), cstr_user, cstr_password);
					delete cstr_user;
					delete cstr_password;

					if (error != sp_error::SP_ERROR_OK) {
						throw gcnew Exception(String::Format("Could not obtain scrobbling possibility: {0}", static_cast<SpErrorCode>(error)));
					}
				} else {
					throw gcnew Exception("Username and password must not be null or empty.");
				}
			} else {
				throw gcnew Exception("Could not set scrobbling credentials: No session created");
			}
		}


		void Session::setConnectionRules(ConnectionRules connectionRules) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_set_connection_rules(session, static_cast<sp_connection_rules>(connectionRules));

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set connection rules: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set connection rules: No session created");
			}
		}

		void Session::setConnectionType(ConnectionType connectionType) {
			if (state >= SessionState::SessionCreated) {
				sp_error error = sp_session_set_connection_type(session, static_cast<sp_connection_type>(connectionType));

				if (error != sp_error::SP_ERROR_OK) {
					throw gcnew Exception(String::Format("Could not set connection type: {0}", static_cast<SpErrorCode>(error)));
				}
			} else {
				throw gcnew Exception("Could not set connection type: No session created");
			}
		}

		Playlist^ Session::createInbox() {
			if (state >= SessionState::LoggedIn) {
				sp_playlist* result = sp_session_inbox_create(session);

				if (result != nullptr) {
					return gcnew Playlist(result);
				}

				return nullptr;
			} else {
				throw gcnew Exception("Could not create inbox playlist: Not logged in");
			}
		}

		Playlist^ Session::createStarred() {
			if (state >= SessionState::LoggedIn) {
				sp_playlist* result = sp_session_starred_create(session);

				if (result != nullptr) {
					return gcnew Playlist(result);
				}

				return nullptr;
			} else {
				throw gcnew Exception("Could not starred playlist: Not logged in");
			}
		}

		Playlist^ Session::createStarredForUser(String^ userCanonicalName) {
			if (state >= SessionState::LoggedIn) {
				if (!String::IsNullOrEmpty(userCanonicalName)) {
					const char* cstr_name = InteropUtilities::convertToCString(userCanonicalName);
					sp_playlist* result = sp_session_starred_for_user_create(session, cstr_name);
					delete cstr_name;

					if (result != nullptr) {
						return gcnew Playlist(result);
					}
				}

				return nullptr;
			} else {
				throw gcnew Exception("Could not starred playlist: Not logged in");
			}
		}

		LibSpotify::PlaylistContainer^ Session::createPublishedContainerForUser(String^ userCanonicalName) {
			if (state >= SessionState::SessionCreated) {
				if (!String::IsNullOrEmpty(userCanonicalName)) {
					const char* cstr_name = InteropUtilities::convertToCString(userCanonicalName);
					sp_playlistcontainer* result = sp_session_publishedcontainer_for_user_create(session, cstr_name);
					delete cstr_name;

					if (result != nullptr) {
						return gcnew LibSpotify::PlaylistContainer(result);
					}
				}

				return nullptr;
			} else {
				throw gcnew Exception("Could not starred playlist: Not session created.");
			}
		}

		/** private **/
		
		int Session::deliver(const sp_audioformat *format, const void *frames, int num_frames) {
			if (num_frames > 0) {
				AudioFormat^ audioFormat = gcnew AudioFormat(format);
				int bufferLength = num_frames * audioFormat->Channels;
				if (deliveryBufferSize < bufferLength) {
					deliveryBufferSize = bufferLength;
					deliveryBuffer = gcnew array<short>(deliveryBufferSize);
				}
				Marshal::Copy(IntPtr((void*)frames), deliveryBuffer, 0, deliveryBufferSize);

				return onMusicDelivery(audioFormat, deliveryBuffer, num_frames);	
			}

			return 0;
		}

		/** player **/

		void Session::playTrack(Track^ track) {
			ActualTrack = track;

			playerLoad(track);
			playerPlay(true);
		}

		void Session::playerLoad(Track^ track) {
			sp_error error = sp_session_player_load(session, track->getPlayable(session));
			if (error != sp_error::SP_ERROR_OK) {
				SpErrorCode errorCode = static_cast<SpErrorCode>(error);
				throw gcnew Exception(String::Format("Could not load track: {0}", errorCode));
			}
		}

		void Session::playerPlay(bool play) {
			sp_error error = sp_session_player_play(session, play);
			if (error != sp_error::SP_ERROR_OK) {
				SpErrorCode errorCode = static_cast<SpErrorCode>(error);
				throw gcnew Exception(String::Format("Could not play/pause track: {0}", errorCode));
			}
		}

		void Session::playerPrefetch(Track^ track) {
			sp_error error = sp_session_player_prefetch(session, track->getPlayable(session));
			if (error != sp_error::SP_ERROR_OK) {
				SpErrorCode errorCode = static_cast<SpErrorCode>(error);
				throw gcnew Exception(String::Format("Could not prefetch track: {0}", errorCode));
			}
		}

		void Session::playerSeek(TimeSpan offset) {
			sp_error error = sp_session_player_seek(session, (int)offset.TotalMilliseconds);
			if (error != sp_error::SP_ERROR_OK) {
				SpErrorCode errorCode = static_cast<SpErrorCode>(error);
				throw gcnew Exception(String::Format("Could not seek track: {0}", errorCode));
			}
		}

		void Session::playerUnload() {
			sp_error error = sp_session_player_unload(session);
			if (error != sp_error::SP_ERROR_OK) {
				SpErrorCode errorCode = static_cast<SpErrorCode>(error);
				throw gcnew Exception(String::Format("Could not unload track: {0}", errorCode));
			}
		}
		
		/** virtual **/
		void Session::onSessionCreated(SpErrorCode errorcode){

		}

		void Session::onSessionReleased(){

		}

		void Session::onLoggedIn(SpErrorCode errorcode){

		}

		void Session::onConnectionError(SpErrorCode errorcode){

		}

		void Session::onLoggedOut(){

		}

		void Session::onPlayTokenLost(){

		}

		void Session::onEndOfTrack(){

		}

		
		void Session::onLogMessage(String^ message){

		}

		void Session::onMessageToUser(String^ message) {

		}

		void Session::onPrivateSessionModeChanged(bool isPrivate) {

		}

		void Session::onGetAudioBufferStats(AudioBufferStats^ stats) {

		}

		void Session::onMetaDataUpdated() {

		}

		void Session::onStreamingError(SpErrorCode errorCode) {

		}

		void Session::onOfflineError(SpErrorCode errorCode) {

		}

		void Session::onConnectionStateUpdated() {

		}

		void Session::onOfflineStatusDataUpdated() {

		}

		void Session::onCredentialsBlobUpdated(String^ blob) {

		}

		void Session::onScrobbleError(SpErrorCode errorCode) {

		}

		/** events handlers **/

		void Session::notifyMainThread(sp_session* session) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->nextEventCall = 0;
				managedSession->processEvent->Set();
			}
		}

		void Session::raiseConnectionError(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];

				SpErrorCode error = static_cast<SpErrorCode>(errorCode);
				managedSession->ConnectionError(managedSession, error);
				managedSession->onConnectionError(error);
			}
		}

		int Session::raiseOnMusicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				return managedSession->deliver(format, frames, num_frames);
			}
		}

		void Session::raiseLoggedIn(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				
				if (errorCode == sp_error::SP_ERROR_OK) {
					sp_user* user = sp_session_user(managedSession->session);
					managedSession->ActiveUser = gcnew User(user);
					managedSession->state = SessionState::LoggedIn;
					managedSession->playlistContainer = gcnew LibSpotify::PlaylistContainer(sp_session_playlistcontainer(session));
				} else {
					managedSession->ActiveUser = nullptr;
					managedSession->state = SessionState::SessionCreated;
				}

				SpErrorCode error = static_cast<SpErrorCode>(errorCode);
				managedSession->LoggedIn(managedSession, error);
				managedSession->onLoggedIn(error);
			}
		}

		void Session::raisePlayTokenLost(sp_session* session){
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];

				managedSession->onPlayTokenLost();
			}
		}

		void Session::raiseEndOfTrack(sp_session* session){
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onEndOfTrack();
			}
		}

		void Session::raiseLoggedOut(sp_session* session){
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->ActiveUser = nullptr;
				managedSession->state = SessionState::SessionCreated;
				managedSession->LoggedOut(managedSession, gcnew EventArgs());
				managedSession->onLoggedOut();
			}
		}

		void Session::raiseLogMessage(sp_session* session, const char* message) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->LogMessage(managedSession, InteropUtilities::convertToString(message));
				managedSession->onLogMessage(InteropUtilities::convertToString(message));
			}
		}

		void Session::raiseMessageToUser(sp_session* session, const char* data) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onMessageToUser(InteropUtilities::convertToString(data));
			}
		}

		void Session::raiseStreamingError(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onStreamingError(static_cast<SpErrorCode>(errorCode));
			}
		}

		void Session::raiseOfflineError(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onOfflineError(static_cast<SpErrorCode>(errorCode));
			}
		}

		void Session::raiseConnectionStateUpdated(sp_session* session) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onConnectionStateUpdated();
			}
		}

		void Session::raisePrivateSessionModeChanged(sp_session* session, bool is_private) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onPrivateSessionModeChanged(is_private);
			}
		}

		void Session::raiseGetAudioBufferStats(sp_session* session, sp_audio_buffer_stats *stats) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onGetAudioBufferStats(gcnew AudioBufferStats(stats));
			}
		}

		void Session::raiseMetaDataUpdated(sp_session* session) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onMetaDataUpdated();
			}
		}

		void Session::raisOfflineStatusDataUpdated(sp_session* session) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onOfflineStatusDataUpdated();
			}
		}

		void Session::raiseCredentialsBlobUpdated(sp_session* session, const char* blob) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onCredentialsBlobUpdated(InteropUtilities::convertToString(blob));
			}
		}

		void Session::raiseScrobbleError(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->onScrobbleError(static_cast<SpErrorCode>(errorCode));
			}
		}

	}
}