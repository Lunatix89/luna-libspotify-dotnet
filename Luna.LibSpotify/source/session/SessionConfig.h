#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include "../user/User.h";
#include "../InteropUtilities.h";
#include <stdint.h>

using namespace System;
using namespace System::Runtime::InteropServices;


namespace Luna {
	namespace LibSpotify {
		ref class Session;
		public ref struct SessionConfig {
		internal:
			sp_session_config* unmanagedPointer;
			sp_session_callbacks* callbacks;
			SessionConfig(sp_session_config* unmanagedPointer);

		private:
			void init();

		public:
			SessionConfig();
			~SessionConfig();

			property int ApiVersion {
				int get(){
					return unmanagedPointer->api_version;
				}
				void set(int value){
					unmanagedPointer->api_version = value;
				}
			}

			property array<Byte>^ ApplicationKey {
				array<Byte>^ get(){
					if (unmanagedPointer->application_key != nullptr){
						array<Byte>^ managed = gcnew array<Byte>(unmanagedPointer->application_key_size);
						Marshal::Copy(IntPtr((void*)unmanagedPointer->application_key), managed, 0, managed->Length);

						return managed;
					}
					
					return nullptr;
				}
				void set(array<Byte>^ value){
					pin_ptr<System::Byte> p = &value[0];
					unmanagedPointer->application_key = reinterpret_cast<const char*>((const char*)p);
					unmanagedPointer->application_key_size = value->Length;
				}
			}

			property String^ CacheLocation {
				String^ get(){
					return gcnew String((const char*)unmanagedPointer->cache_location);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					unmanagedPointer->cache_location = static_cast<const char*>(p.ToPointer());
				}
			}

			property String^ SettingsLocation {
				String^ get(){
					return gcnew String((const char*)unmanagedPointer->settings_location);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					unmanagedPointer->settings_location = static_cast<const char*>(p.ToPointer());
				}
			}
						
			property String^ UserAgent {
				String^ get(){
					return gcnew String((const char*)unmanagedPointer->user_agent);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					unmanagedPointer->user_agent = static_cast<const char*>(p.ToPointer());
				}
			}
			
            //public IntPtr callbacks;					
            //public IntPtr userdata;
				
			property bool CompressPlaylists {
				bool get(){
					return unmanagedPointer->compress_playlists;
				}
				void set(bool value){
					unmanagedPointer->compress_playlists = value;
				}
			}

			property bool DontSaveMetadataForPlaylists {
				bool get(){
					return unmanagedPointer->dont_save_metadata_for_playlists;
				}
				void set(bool value){
					unmanagedPointer->dont_save_metadata_for_playlists = value;
				}
			}

			property bool InitiallyUnloadPlaylists {
				bool get(){
					return unmanagedPointer->initially_unload_playlists;
				}
				void set(bool value){
					unmanagedPointer->initially_unload_playlists = value;
				}
			}
		};
	}
}