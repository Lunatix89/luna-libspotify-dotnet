#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class Playlist {
		internal:
			sp_playlist* playlist;

			Playlist(sp_playlist* playlist) {
				this->playlist = playlist;
			}
		private:
			Playlist() {
				this->playlist = nullptr;
			}

		public:

			~Playlist() {

			}

		};
	}
}