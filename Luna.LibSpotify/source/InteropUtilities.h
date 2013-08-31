#pragma once
#include <string.h>
#include <memory>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Luna {
	
	public ref class InteropUtilities {

		public:
			static String^ convertToString(const char* string) {
				return gcnew String(string);
			}

			static const char* convertToCString(String^ string) {
				IntPtr p = Marshal::StringToHGlobalAnsi(string);
				
				const char* cast = static_cast<const char*>(p.ToPointer());
				char* result = new char[string->Length + 1];
				memset(result, 0, string->Length + 1);
				memcpy(result, cast, string->Length);
				Marshal::FreeHGlobal(p);

				return (const char*)result;
			}

	};

}