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

			property int NumTracks {
				int get() {
					if (playlist != nullptr) {
						return sp_playlist_num_tracks(playlist);
					}
					return 0;
				}
			}

			Track^ getTrack(int index) {
				if ((index < 0) || (index >= NumTracks)) {
					throw gcnew IndexOutOfRangeException();
				}
				
				sp_track* track = sp_playlist_track(playlist, index);
				if (track != nullptr) {
					return gcnew Track(track);
				}

				return nullptr;
			}
		};
	}
}