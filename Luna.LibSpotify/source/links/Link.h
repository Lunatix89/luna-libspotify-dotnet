#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {

		ref class Track;
		public ref class Link {
		internal:
			sp_link* link;
			
			Link(sp_link* link);
		private:
			

		public:
			Link();
			~Link();

			property String^ Url {
				String^ get();
				void set(String^ value);
			}

			Track^ asTrack();
			Track^ asTrackAndOffset();
			
			static Link^ createFromString(String^ url);
			static Link^ createFromTrack(Track^ track, TimeSpan offset);

		};

	}
}