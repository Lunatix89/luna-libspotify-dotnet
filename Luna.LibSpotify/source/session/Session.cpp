#include <stdint.h>
#include <memory>
#include "Session.h";
#include "../track/Track.h"
#include "AudioFormat.h"


using namespace System;
using namespace System::Collections::Generic;

namespace Luna {
	namespace LibSpotify {

		
		Session::Session(){
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
			UserInfo^ userInfo = gcnew UserInfo();
			userInfo->Username = username;
			userInfo->Password = password;

			invoke(gcnew Action<Object^>(this, &Session::loginInternal), userInfo, true);
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
			invoke(gcnew Action(this, &Session::logoutInternal), false);
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
			invoke(gcnew Action(this, &Session::releaseSessionInternal), false);
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
					Monitor::Exit(processLock);
				}
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
		/*
		int Session::onMusicDelivery(AudioFormat^ audioFormat, array<short>^ pcmData, int numFrames){

		}
		*/
		void Session::onPlayTokenLost(){

		}

		void Session::onEndOfTrack(){

		}

		
		void Session::onLogMessage(String^ message){

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
				} else {
					managedSession->ActiveUser = nullptr;
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
	}
}