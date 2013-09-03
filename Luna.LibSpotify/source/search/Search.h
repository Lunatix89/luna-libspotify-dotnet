#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
		public ref class Search {
		internal:
			sp_search* search;

			Search(sp_search* search) {
				this->search = search;
			}
		private:
			Search() {
				this->search = nullptr;
			}

		public:

			~Search() {

			}

		};
	}
}