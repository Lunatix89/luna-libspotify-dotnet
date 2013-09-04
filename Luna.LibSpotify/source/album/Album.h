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

			property bool IsLoaded {
				bool get() {
					if (album != nullptr) {
						return sp_album_is_loaded(album);
					}
					return 0;
				}
			}

			property bool IsAvailable {
				bool get() {
					if (album != nullptr) {
						return sp_album_is_available(album);
					}
					return 0;
				}
			}

			property String^ Name {
				String^ get() {
					if (album != nullptr) {
						return InteropUtilities::convertToString(sp_album_name(album));
					}
					return String::Empty;
				}
			}
			/*
			property LibSpotify::AlbumType AlbumType {
				LibSpotify::AlbumType get() {
					if (album != nullptr) {
						return static_cast<LibSpotify::AlbumType>(sp_album_type(album));
					}
					return LibSpotify::AlbumType::None;
				}
			}*/

		};
	}
}