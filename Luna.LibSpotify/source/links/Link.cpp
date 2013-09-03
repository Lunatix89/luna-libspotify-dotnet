#include "Link.h"
#include "../track/Track.h";
#include "../album/Album.h"
#include "../artist/Artist.h"
#include "../artist/ArtistBrowse.h"
#include "../search/Search.h"
#include "../user/User.h"
#include "../playlist/Playlist.h"
#include "../image/Image.h"

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
			if ((LinkType == LibSpotify::LinkType::LocalTrack) || (LinkType == LibSpotify::LinkType::Track)) {
				sp_track* track = sp_link_as_track(link);
				return gcnew Track(track);
			} else {
				throw gcnew Exception("Link is not a track or a local track link");
			}			
		}

		Track^ Link::asTrackAndOffset() {
			if ((LinkType == LibSpotify::LinkType::LocalTrack) || (LinkType == LibSpotify::LinkType::Track)) {
				int offset = 0;
				sp_track* track = sp_link_as_track_and_offset(link, &offset);
				return gcnew Track(track);
			} else {
				throw gcnew Exception("Link is not a track or a local track link");
			}	
		}

		Album^ Link::asAlbum() {
			if (LinkType == LibSpotify::LinkType::Artist) {
				sp_album* album = sp_link_as_album(link);
				return gcnew Album(album);
			} else {
				throw gcnew Exception("Link is not an album link.");
			}		
		}

		Artist^ Link::asArtist() {
			if (LinkType == LibSpotify::LinkType::Artist) {
				sp_artist* artist = sp_link_as_artist(link);
				return gcnew Artist(artist);
			} else {
				throw gcnew Exception("Link is not an artist link.");
			}	
		}

		User^ Link::asUser() {
			if (LinkType == LibSpotify::LinkType::Profile) {
				sp_user* user = sp_link_as_user(link);
				return gcnew User(user);
			} else {
				throw gcnew Exception("Link is not a user link.");
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

		Link^ Link::createFromAlbum(Album^ url) {
			sp_link* link = sp_link_create_from_album(url->album);
			return gcnew Link(link);
		}
		
		Link^ Link::createFromAlbumCover(Album^ url, ImageSize imageSize) {
			sp_link* link = sp_link_create_from_album_cover(url->album, static_cast<sp_image_size>(imageSize));
			return gcnew Link(link);
		}
		
		Link^ Link::createFromArtist(Artist^ url) {
			sp_link* link = sp_link_create_from_artist(url->artist);
			return gcnew Link(link);
		}
		
		Link^ Link::createFromArtistPortrait(Artist^ url, ImageSize imageSize) {
			sp_link* link = sp_link_create_from_artist_portrait(url->artist, static_cast<sp_image_size>(imageSize));
			return gcnew Link(link);
		}
		
		Link^ Link::createFromArtistBrowsePortait(ArtistBrowse^ url, ImageSize imageSize) {
			sp_link* link = sp_link_create_from_artistbrowse_portrait(url->artistBrowse, static_cast<sp_image_size>(imageSize));
			return gcnew Link(link);
		}
		
		Link^ Link::createFromSearch(Search^ url) {
			sp_link* link = sp_link_create_from_search(url->search);
			return gcnew Link(link);
		}
		
		Link^ Link::createFromPlaylist(Playlist^ url) {
			sp_link* link = sp_link_create_from_playlist(url->playlist);
			return gcnew Link(link);
		}
		
		Link^ Link::createFromUser(User^ url) {
			sp_link* link = sp_link_create_from_user(url->user);
			return gcnew Link(link);
		}
		
		Link^ Link::createFromImage(SpotifyImage^ url) {
			sp_link* link = sp_link_create_from_image(url->image);
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

		// Create a Spotify link given a string
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