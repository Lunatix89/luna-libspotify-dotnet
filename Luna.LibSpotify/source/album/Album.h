#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class Album {
		internal:
			sp_album* album;

			Album(sp_album* album) {
				this->album = album;
			}
		private:
			Album() {
				this->album = nullptr;
			}

		public:

			~Album() {

			}

		};
	}
}