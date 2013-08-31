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
			sp_audio_buffer_stats* nativeStruct;

		public:
			property int Samples {
				int get(){
					return nativeStruct->samples;
				}

				void set(int value){
					nativeStruct->samples = value;
				}
			}

            property int Stutter{
				int get(){
					return nativeStruct->stutter;
				}
				void set(int value){
					nativeStruct->stutter = value;
				}
			}
        };
	
	}
}