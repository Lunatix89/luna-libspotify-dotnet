#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class SpotifyImage {
		internal:
			sp_image* image;

			SpotifyImage(sp_image* image) {
				this->image = image;
			}
		private:
			SpotifyImage() {
				this->image = nullptr;
			}

		public:

			~SpotifyImage() {

			}

		};
	}
}