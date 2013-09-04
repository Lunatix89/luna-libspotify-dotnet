#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include "../session/Session.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {

		ref class Album;
		ref class Artist;
		public ref class Track {
		internal:
			sp_track* track;
			sp_artist* getArtistInternal(int index);
			sp_album* getAlbumInternal();

			Track(sp_track* track);
			
			sp_track* getPlayable(sp_session* session);
			sp_track* getPlayable(Session^ session);
			void release();
		private:
			Track();
		public:
			~Track();

			static Track^ createLocal(String^ artist, String^ album, String^ name, int duration);

			property bool IsLoaded {
				bool get() {
					return track != nullptr ? sp_track_is_loaded(track) : false;
				}
			}

			property SpErrorCode IsErrornous {
				SpErrorCode get(){
					return track != nullptr ? static_cast<SpErrorCode>(sp_track_error(track)) : SpErrorCode::TrackNotAvailable;
				}
			}

			bool isLocal(Session^ session) {
				if ((session != nullptr) && (track != nullptr)) {
					return track != nullptr ? sp_track_is_local(session->session, track) : false;
				}
				return false;
			}

			bool isPlayable(Session^ session) {
				if ((session != nullptr) && (track != nullptr)) {
					return track != nullptr ? sp_track_is_local(session->session, track) : false;
				}
				return false;
			}

			int numArtists(Session^ session) {
				if ((session != nullptr) && (track != nullptr)) {
					return track != nullptr ? sp_track_num_artists(track) : 0;
				}
				return 0;
			}

			property String^ Name {
				String^ get() {
					return track != nullptr ? InteropUtilities::convertToString(sp_track_name(track)) : String::Empty;
				}
			}

			property TimeSpan Duration {
				TimeSpan get() {
					return track != nullptr ? TimeSpan::FromMilliseconds(sp_track_duration(track)) : TimeSpan::Zero;
				}
			}

			property int Popularity {
				int get() {
					return track != nullptr ? sp_track_popularity(track) : 0;
				}
			}

			property int Disc {
				int get() {
					return track != nullptr ? sp_track_disc(track) : 0;
				}
			}

			property int Index {
				int get() {
					return track != nullptr ? sp_track_index(track) : 0;
				}
			}

			
			property LibSpotify::Album^ Album {
				LibSpotify::Album^ get();
			}
			
			Artist^ getArtist(int index);
			


		};

	}
}