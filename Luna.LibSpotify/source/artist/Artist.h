#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class Artist {
		internal:
			sp_artist* artist;

			Artist(sp_artist* artist) {
				this->artist = artist;
			}
		private:
			Artist() {
				this->artist = nullptr;
			}

		public:

			~Artist() {

			}

		};
	}
}