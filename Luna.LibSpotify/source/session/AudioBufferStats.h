#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include <stdint.h>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	namespace LibSpotify {
        public ref struct AudioBufferStats {
		internal:
			sp_audio_buffer_stats* stats;

		public:
			property int Samples {
				int get(){
					return stats->samples;
				}

				void set(int value){
					stats->samples = value;
				}
			}

            property int Stutter{
				int get(){
					return stats->stutter;
				}
				void set(int value){
					stats->stutter = value;
				}
			}
        };
	
	}
}