#include "AudioFormat.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Luna {
	namespace LibSpotify {

		AudioFormat::AudioFormat() {
			this->unmanagedPointer = nullptr;
		}

		AudioFormat::AudioFormat(const sp_audioformat* format) {
			this->unmanagedPointer = (sp_audioformat*)format;
		}

		AudioFormat::~AudioFormat() {

		}

	}
}
