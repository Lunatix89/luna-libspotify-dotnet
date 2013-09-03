#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {

		ref class Album;
		ref class Artist;
		ref class ArtistBrowse;
		ref class Search;
		ref class User;
		ref class Playlist;
		ref class SpotifyImage;

		ref class Track;
		public ref class Link {
		internal:
			sp_link* link;
			
			Link(sp_link* link);
		private:
			

		public:
			Link();
			~Link();

			// the url of the link
			property String^ Url {
				String^ get();
				void set(String^ value);
			}

			/// <summary>
			/// The link type of the link
			/// </summary>
			property LibSpotify::LinkType LinkType {
				LibSpotify::LinkType get() {
					if (link != nullptr) {
						return static_cast<LibSpotify::LinkType>(sp_link_type(link));
					}

					return LibSpotify::LinkType::Invalid;
				}
			}
			/// <summary>
			/// converts the link to a track
			/// </summary>
			Track^ asTrack();
			// converts the link to a track with offset
			Track^ asTrackAndOffset();
			// converts the link to an album
			Album^ asAlbum();
			// converts the link to am artist
			Artist^ asArtist();
			// converts the link to a user
			User^ asUser();

			// creates a link from a string (spotify url)
			static Link^ createFromString(String^ url);
			// creates a link from a track and an offset
			static Link^ createFromTrack(Track^ track, TimeSpan offset);
			// creates a link from an album
			static Link^ createFromAlbum(Album^ url);
			// creates a link from the albums cover
			static Link^ createFromAlbumCover(Album^ url, ImageSize imageSize);
			// creates a link from an artist
			static Link^ createFromArtist(Artist^ url);
			// creats a link from an artists portrait
			static Link^ createFromArtistPortrait(Artist^ url, ImageSize imageSize);
			// creats a link from an artist browse portrait
			static Link^ createFromArtistBrowsePortait(ArtistBrowse^ url, ImageSize imageSize);
			// creats a link from a search
			static Link^ createFromSearch(Search^ url);
			// creats a link from a playlist
			static Link^ createFromPlaylist(Playlist^ url);
			// creats a link from a user
			static Link^ createFromUser(User^ url);
			// creats a link from an image
			static Link^ createFromImage(SpotifyImage^ url);
			
		};

	}
}