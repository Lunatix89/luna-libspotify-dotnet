#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include "SessionConfig.h"
#include "../user/User.h";
#include "../InteropUtilities.h";
#include <stdint.h>

using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;


namespace Luna {
	namespace LibSpotify {

		public enum class SessionState {
			None,
			SessionCreated,
			LoggedIn,
			LoggedOut,
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
			static const int initialDeliveryBufferSize = 1024;

		private:
			bool isDisposed;
			SessionState^ state;
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

			property User^ ActiveUser {
				User^ get() {
					return activeUser;
				}
				private:
				void set(User^ value) {
					activeUser = value;
				}
			}

			void createSession();
			void login(String^ username, String^ password);
			void logout();
			void releaseSession();

			void playerLoad(Track^ track);
			void playerPlay(bool play);
			void playerPrefetch(Track^ track);
			void playerSeek(TimeSpan offset);
			void playerUnload();

			virtual void onSessionCreated(SpErrorCode errorcode);
			virtual void onSessionReleased();
			virtual void onLoggedIn(SpErrorCode errorcode);
			virtual void onConnectionError(SpErrorCode errorcode);
			virtual void onLoggedOut();
			virtual int onMusicDelivery(AudioFormat^ audioFormat, array<short>^ pcmData, int numFrames) = 0;
			virtual void onPlayTokenLost();
			virtual void onEndOfTrack();
			virtual void onLogMessage(String^ message);

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