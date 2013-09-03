#include "User.h"

using namespace System;

namespace Luna {
	namespace LibSpotify {

		User::User() {
			user = nullptr;
		}

		User::User(sp_user* user) {
			this->user = user;
		}

		User::~User(){

		}

	}
}

