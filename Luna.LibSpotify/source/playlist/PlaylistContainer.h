#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class PlaylistContainer {
		internal:
			sp_playlistcontainer* playlistContainer;

			PlaylistContainer(sp_playlistcontainer* playlistContainer) {
				this->playlistContainer = playlistContainer;
			}
		private:
			PlaylistContainer() {
				this->playlistContainer = nullptr;
			}

		public:

			~PlaylistContainer() {

			}

		};
	}
}