#include "Track.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {

		/** Internal **/

		Track::Track(sp_track* track) {
			this->track = track;
		}

		sp_track* Track::getPlayable(Session^ session){
			return sp_track_get_playable(session->session, track);
		}

		sp_track* Track::getPlayable(sp_session* session) {
			return sp_track_get_playable(session, track);
		}

		sp_artist* Track::getArtistInternal(int index) {
			return (track != nullptr ? sp_track_artist(track, index) : nullptr);
		}

		sp_album* Track::getAlbumInternal() {
			return (track != nullptr ? sp_track_album(track) : nullptr);
		}

		void Track::release() {
			if (track != nullptr) {
				sp_track_release(track);
			}
		}

		/** private **/

		Track::Track() {
			track = nullptr;
		}

		/** public **/

		Track::~Track() {
			release();
		}

		Track^ Track::createLocal(String^ artist, String^ album, String^ title, int duration) {
			const char* cstr_artist = InteropUtilities::convertToCString(artist);
			const char* cstr_album = InteropUtilities::convertToCString(album);
			const char* cstr_title = InteropUtilities::convertToCString(title);

			sp_track* track = sp_localtrack_create(cstr_artist, cstr_title, cstr_album, duration);
			
			if (track != nullptr) {
				return gcnew Track(track);
			}

			return nullptr;
		}

	}
}