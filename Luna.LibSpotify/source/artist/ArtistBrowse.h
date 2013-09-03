#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class ArtistBrowse {
		internal:
			sp_artistbrowse* artistBrowse;

			ArtistBrowse(sp_artistbrowse* artistBrowse) {
				this->artistBrowse = artistBrowse;
			}
		private:
			ArtistBrowse() {
				this->artistBrowse = nullptr;
			}

		public:

			~ArtistBrowse() {

			}

		};
	}
}