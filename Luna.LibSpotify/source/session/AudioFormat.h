#pragma once
#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include <stdint.h>

using namespace System;
using namespace System::Runtime::InteropServices;


namespace Luna {
	namespace LibSpotify {
		public ref struct AudioFormat {
		internal:
			sp_audioformat* unmanagedPointer;
		//	AudioFormat(sp_audioformat* audioFormat);

		public:
		//	~AudioFormat();

			property LibSpotify::SampleType SampleType{
				LibSpotify::SampleType get(){
					return static_cast<LibSpotify::SampleType>(unmanagedPointer->sample_type);
				}
				void set(LibSpotify::SampleType value){
					unmanagedPointer->sample_type = static_cast<sp_sampletype>(value);
				}
			}

			property int SampleRate{
				int get(){
					return unmanagedPointer->sample_rate;
				}
				void set(int value){
					unmanagedPointer->sample_rate = value;
				}
			}

			property int Channels{
				int get(){
					return unmanagedPointer->channels;
				}
				void set(int value){
					unmanagedPointer->channels = value;
				}
			}
        };
	}
}