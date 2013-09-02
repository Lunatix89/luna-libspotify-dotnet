#include "Link.h"
#include "../track/Track.h";


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {

		/** Internal **/

		Link::Link(sp_link* link) {
			this->link = link;
		}

		/** private **/


		/** public **/

		Link::Link() {
			this->link = nullptr;
		}

		Link::~Link() {
			if (link != nullptr) {
				sp_link_release(link);
			}
		}

		Track^ Link::asTrack() {
			if (link != nullptr) {
				sp_track* track = sp_link_as_track(link);
				return gcnew Track(track);
			} else {
				throw gcnew Exception("Link is empty");
			}			
		}

		Track^ Link::asTrackAndOffset() {
			if (link != nullptr) {
				int offset = 0;
				sp_track* track = sp_link_as_track_and_offset(link, &offset);
				return gcnew Track(track);
			} else {
				throw gcnew Exception("Link is empty");
			}	
		}

			
		Link^ Link::createFromString(String^ url) {
			Link^ link = gcnew Link();
			link->Url = url;

			return link;
		}

		Link^ Link::createFromTrack(Track^ track, TimeSpan offset) {
			sp_link* link = sp_link_create_from_track(track->track, (int)offset.TotalMilliseconds);

			return gcnew Link(link);
		}
		
		String^ Link::Url::get() {
			if (link != nullptr){
				char url[256];
				sp_link_as_string(link, url, sizeof(url));

				return InteropUtilities::convertToString((const char*)url);
			}

			return String::Empty;
		}

		void Link::Url::set(String^ value) {
			if (link != nullptr) {
				sp_link_release(link);
				link = nullptr;
			}

			if (!String::IsNullOrEmpty(value)) {
				const char* cstr_url = InteropUtilities::convertToCString(value);
				link = sp_link_create_from_string(cstr_url);
				delete cstr_url;
			}
		}

	}
}