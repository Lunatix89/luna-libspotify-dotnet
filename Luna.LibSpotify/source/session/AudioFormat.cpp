#include "AudioFormat.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Luna {
	namespace LibSpotify {

		AudioFormat::AudioFormat() {
			this->audioformat = nullptr;
		}

		AudioFormat::AudioFormat(const sp_audioformat* format) {
			this->audioformat = (sp_audioformat*)format;
		}

		AudioFormat::~AudioFormat() {

		}

	}
}
