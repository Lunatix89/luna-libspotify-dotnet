#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include "SessionConfig.h"
#include "../user/User.h";
#include "../InteropUtilities.h";
#include "AudioBufferStats.h"
#include <stdint.h>

using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;


namespace Luna {
	namespace LibSpotify {

		public enum class SessionState {
			None = 0,
			SessionCreated = 1,
			LoggedIn = 2,
		};

		ref struct ActionInfo {
			public:
				Object^ action;
				Object^ arg;
				bool needsValidSession;
		};

		ref struct UserInfo {
			public:
				String^ Username;
				String^ Password;
		};

		ref class PlaylistContainer;
		ref class Playlist;
		ref class Track;
		ref class User;
		ref struct AudioFormat;
		public ref class Session abstract {
		internal:
			sp_session* session;
			static Dictionary<int, Session^>^ sessionTable = gcnew Dictionary<int, Session^>();

			property int SessionId {
				int get(){
					return sessionId;
				}
			}

			static int raiseOnMusicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames);
			static void raisePlayTokenLost(sp_session* session);
			static void raiseEndOfTrack(sp_session* session);
			static void raiseLoggedIn(sp_session* session, sp_error errorCode);
			static void raiseLoggedOut(sp_session* session);
			static void raiseLogMessage(sp_session* session, const char* data);
			static void raiseConnectionError(sp_session* session, sp_error errorCode);
			static void notifyMainThread(sp_session* session);
			static void raiseMessageToUser(sp_session* session, const char* data);
			static void raiseStreamingError(sp_session* session, sp_error errorCode);
			static void raiseOfflineError(sp_session* session, sp_error errorCode);
			static void raiseConnectionStateUpdated(sp_session* session);
			static void raisePrivateSessionModeChanged(sp_session* session, bool is_private);
			static void raiseGetAudioBufferStats(sp_session* session, sp_audio_buffer_stats *stats);
			static void raiseMetaDataUpdated(sp_session* session);
			static void raisOfflineStatusDataUpdated(sp_session* session);
			static void raiseCredentialsBlobUpdated(sp_session* session, const char* blob);
			static void raiseScrobbleError(sp_session* session, sp_error errorCode);
			
			static const int initialDeliveryBufferSize = 1024;

		private:
			Track^ actualTrack;
			bool isDisposed;
			SessionState state;
			AutoResetEvent^ processEvent;
			int nextEventCall;
			Task^ sessionTask;
			LibSpotify::SessionConfig^ sessionConfig;
			int sessionId;
			Object^ processLock;
			List<ActionInfo^>^ actionStack;
			User^ activeUser;
			array<short>^ deliveryBuffer;
			int deliveryBufferSize;
			LibSpotify::PlaylistContainer^ playlistContainer;

			void sessionRunner(Object^ state);
			
			void invoke(Action^ action, bool needsActiveSession);
			void invoke(Action<Object^>^ action, Object^ arg, bool needsActiveSession);
			void createSessionInternal();
			void loginInternal(Object^ arg);
			void logoutInternal();
			void releaseSessionInternal();
			int deliver(const sp_audioformat *format, const void *frames, int num_frames);
		public:
			Session();
			~Session();

			event EventHandler<SpErrorCode>^ SessionCreated;
			event EventHandler^ SessionStopped;
			event EventHandler<SpErrorCode>^ LoggedIn;
			event EventHandler^ LoggedOut;
			event EventHandler<String^>^ LogMessage;
			event EventHandler<SpErrorCode>^ ConnectionError;

			property bool IsSessionCreated {
				bool get() {
					return (state >= SessionState::SessionCreated);
				}
			}

			property bool IsLoggedIn {
				bool get() {
					return (state >= SessionState::LoggedIn);
				}
			}

			property User^ ActiveUser {
				User^ get() {
					return activeUser;
				}
				private:
				void set(User^ value) {
					activeUser = value;
				}
			}
						
			property bool IsPrivateSession {
				bool get() {
					if (session != nullptr) {
						return sp_session_is_private_session(session);
					}
					return false;
				}
				void set(bool value) {
					if (session != nullptr) {
						sp_session_set_private_session(session, value);
					}
				}
			}

			property bool VolumeNormalization {
				bool get() {
					if (session != nullptr) {
						return sp_session_get_volume_normalization(session);
					}
					return false;
				}
				void set(bool value) {
					if (session != nullptr) {
						sp_session_set_volume_normalization(session, value);
					}
				}
			}

			property int OfflineTracksToSync {
				int get() {
					if (session != nullptr) {
						return sp_offline_tracks_to_sync(session);
					}
					return 0;
				}
			}

			property int NumOfflinePlaylists {
				int get() {
					if (session != nullptr) {
						return sp_offline_num_playlists(session);
					}
					return 0;
				}
			}

			property Track^ ActualTrack {
				Track^ get() {
					return actualTrack;
				}
				private:
				void set(Track^ value) {
					actualTrack = value;
				}
			}

			property double PlaybackPosition  {
				virtual double get() abstract;
				virtual void set(double value) abstract;
			}

			property TimeSpan OfflineTimeLeft {
				TimeSpan get() {
					if (session != nullptr) {
						return TimeSpan::FromSeconds(sp_offline_time_left(session));
					}
					return TimeSpan::Zero;
				}
			}

			property int UserCountry {
				int get() {
					if (session != nullptr) {
						return sp_session_user_country(session);
					}

					return 0;
				}
			}

			property LibSpotify::PlaylistContainer^ PlaylistContainer {
				LibSpotify::PlaylistContainer^ get() {
					return playlistContainer;
				}
				private:
				void set(LibSpotify::PlaylistContainer^ value) {
					playlistContainer = value;
				}
			}
				
			
			void createSession();
			void login(String^ username, String^ password);
			void logout();			
			void releaseSession();

			void forgetme();
			void flushCaches();
			void setCacheSize(int size);
			void setPreferredBitrate(Bitrate bitrate);
			void setPreferredBitrates(Bitrate onlineBitrate, Bitrate offlineBitrate, bool resynchronize);
			void setPreferredOfflineBitrate(Bitrate bitrate, bool resynchronize);
			void setConnectionRules(ConnectionRules connectionRules);
			void setConnectionType(ConnectionType connectionType);
			Playlist^ createInbox();
			Playlist^ createStarred();
			Playlist^ createStarredForUser(String^ userCanonicalName);
			LibSpotify::PlaylistContainer^ createPublishedContainerForUser(String^ userCanonicalName);
			void setScrobbling(SocialProvider provider, ScrobblingState state);
			ScrobblingState getScrobblingState(SocialProvider provider);
			bool isScrobblingPossible(SocialProvider provider);
			void setScrobblingCredentials(SocialProvider provider, String^ username, String^ password);

			void playerLoad(Track^ track);
			void playerPlay(bool play);
			void playerPrefetch(Track^ track);
			void playerSeek(TimeSpan offset);
			void playerUnload();

			virtual void playTrack(Track^ track);

			virtual void onSessionCreated(SpErrorCode errorcode);
			virtual void onSessionReleased();
			virtual void onLoggedIn(SpErrorCode errorcode);
			virtual void onConnectionError(SpErrorCode errorcode);
			virtual void onLoggedOut();
			virtual int onMusicDelivery(AudioFormat^ audioFormat, array<short>^ pcmData, int numFrames) = 0;
			virtual void onPlayTokenLost();
			virtual void onEndOfTrack();
			virtual void onLogMessage(String^ message);
			virtual void onMessageToUser(String^ message);
			virtual void onPrivateSessionModeChanged(bool isPrivate);
			virtual void onGetAudioBufferStats(AudioBufferStats^ stats);
			virtual void onMetaDataUpdated();
			virtual void onStreamingError(SpErrorCode errorCode);
			virtual void onOfflineError(SpErrorCode errorCode);
			virtual void onConnectionStateUpdated();
			virtual void onOfflineStatusDataUpdated();
			virtual void onCredentialsBlobUpdated(String^ blob);
			virtual void onScrobbleError(SpErrorCode errorCode);

			property LibSpotify::SessionConfig^ SessionConfig {
				LibSpotify::SessionConfig^ get(){
					return sessionConfig;
				}
				void set(LibSpotify::SessionConfig^ value){
					sessionConfig = value;
				}
			}
		};

	}
}