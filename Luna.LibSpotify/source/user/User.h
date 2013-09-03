#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"

using namespace System;

namespace Luna {
	namespace LibSpotify {

		public enum class RelationType {
			Unknown = 0,
			None = 1,
			UniDirectional = 2,
			BiDirectional = 3,
		};

		public ref class User {
		internal:
			User(sp_user* user);
			sp_user* user;

		private:
			User();
		public:
			~User();

			property bool IsLoaded{
				bool get() {
					return sp_user_is_loaded(user);
				}
			}

			property String^ CanonicalName{
				String^ get() {
					if (user != nullptr) {
						const char* name = sp_user_canonical_name(user);
						return gcnew String(name);
					}

					return String::Empty;
				}
			}

			property String^ DisplayName {
				String^ get(){
					if (user != nullptr) {
						const char* name = sp_user_display_name(user);
						return gcnew String(name);
					}

					return String::Empty;
				}
			}

		};
	}
}