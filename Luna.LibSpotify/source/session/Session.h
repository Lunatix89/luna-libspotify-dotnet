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

		ref class User;
		public ref class Session {
		internal:
			sp_session* unmanagedPointer;
			static Dictionary<int, Session^>^ sessionTable = gcnew Dictionary<int, Session^>();

			property int SessionId {
				int get(){
					return sessionId;
				}
			}

			static void raiseLoggedIn(sp_session* session, sp_error errorCode);
			static void raiseLoggedOut(sp_session* session);
			static void raiseLogMessage(sp_session* session, const char* data);
			static void raiseConnectionError(sp_session* session, sp_error errorCode);
			static void notifyMainThread(sp_session* session);
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


			void sessionRunner(Object^ state);
			
			void invoke(Action^ action, bool needsActiveSession);
			void invoke(Action<Object^>^ action, Object^ arg, bool needsActiveSession);
			void createSessionInternal();
			void loginInternal(Object^ arg);
			void logoutInternal();
			void releaseSessionInternal();
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