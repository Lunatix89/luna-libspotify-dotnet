#include <stdint.h>
#include <memory>
#include "Session.h";



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
			unmanagedPointer = nullptr;
			sessionConfig = gcnew LibSpotify::SessionConfig();
			sessionTask = gcnew Task(gcnew Action<Object^>(this, &Session::sessionRunner), gcnew Object(), CancellationToken::None, System::Threading::Tasks::TaskCreationOptions::LongRunning);
			sessionTask->Start(TaskScheduler::Default);

		}

		Session::~Session(){
			isDisposed = true;

			if (unmanagedPointer != nullptr){
				delete unmanagedPointer;
			}
		}

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
					if ((Environment::TickCount >= nextEventCall) && (unmanagedPointer != nullptr)) {
						Monitor::Enter(processLock);
						try{
							int nextCall = 0;
							sp_session_process_events(unmanagedPointer, &nextCall);
							nextEventCall = nextCall / 1000;
						} finally {
							Monitor::Exit(processLock);
						}

						working = true;
					}
				}
			}
		}

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

		void Session::createSession(){
			invoke(gcnew Action(this, &Session::createSessionInternal), false);
		}

		void Session::createSessionInternal() {
			sp_session* sp;
			sp_error errorCode = sp_session_create(sessionConfig->unmanagedPointer, &sp);
			unmanagedPointer = sp;

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
					
			SessionCreated(this, static_cast<SpErrorCode>(errorCode));
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

				sp_error error = sp_session_login(unmanagedPointer, user, passwd, false, nullptr);
			}
		}

		void Session::logout(){
			invoke(gcnew Action(this, &Session::logoutInternal), false);
		}

		void Session::logoutInternal() {
			if (unmanagedPointer != nullptr) {
				Monitor::Enter(processLock);
				try{
					sp_user_release(ActiveUser->unmanagedPointer);
					sp_error result = sp_session_logout(unmanagedPointer);
					sp_connectionstate state = sp_session_connectionstate(unmanagedPointer);

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
			if (unmanagedPointer != nullptr){
				Monitor::Enter(processLock);
				try{
					sessionTable->Remove(sessionId);
					sp_session_release(unmanagedPointer);
					unmanagedPointer = nullptr;

					SessionStopped(this, gcnew EventArgs());
				} finally {
					Monitor::Exit(processLock);
				}
			}
		}

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
				managedSession->ConnectionError(managedSession, static_cast<SpErrorCode>(errorCode));
			}
		}

		void Session::raiseLoggedIn(sp_session* session, sp_error errorCode) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				
				if (errorCode == sp_error::SP_ERROR_OK) {
					sp_user* user = sp_session_user(managedSession->unmanagedPointer);
					managedSession->ActiveUser = gcnew User(user);
				} else {
					managedSession->ActiveUser = nullptr;
				}

				managedSession->LoggedIn(managedSession, static_cast<SpErrorCode>(errorCode));
			}
		}

		void Session::raiseLoggedOut(sp_session* session){
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->ActiveUser = nullptr;
				managedSession->LoggedOut(managedSession, gcnew EventArgs());
			}
		}

		void Session::raiseLogMessage(sp_session* session, const char* message) {
			int sessionPtr = (int)session;

			if (Session::sessionTable->ContainsKey(sessionPtr)){
				Session^ managedSession = Session::sessionTable[sessionPtr];
				managedSession->LogMessage(managedSession, InteropUtilities::convertToString(message));
			}
		}
	}
}