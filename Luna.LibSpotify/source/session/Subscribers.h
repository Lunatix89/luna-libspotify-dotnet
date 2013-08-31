#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include <stdint.h>

using namespace System;

namespace Luna {
	namespace LibSpotify {		
		public ref struct Subscribers {
		internal:
			sp_subscribers* unmanagedPointer;

		public:
			 property int Subscriber {
				int get(){
					return 0;
				}
				void set(int value){
					unmanagedPointer->count = value;
				}
			}

			 property int Count {
				int get(){
					return unmanagedPointer->count;
				}
				void set(int value){
					unmanagedPointer->count = value;
				}
			}
        };
	}
}