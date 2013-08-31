#include "User.h"

using namespace System;

namespace Luna {
	namespace LibSpotify {

		User::User(sp_user* user) {
			unmanagedPointer = user;
		}

		User::~User(){

		}

	}
}

