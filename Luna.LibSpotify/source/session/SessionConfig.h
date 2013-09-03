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
			sp_session_config* sessionConfig;
			sp_session_callbacks* callbacks;
			SessionConfig(sp_session_config* sessionConfig);

		private:
			void init();

		public:
			SessionConfig();
			~SessionConfig();

			property int ApiVersion {
				int get(){
					return sessionConfig->api_version;
				}
				void set(int value){
					sessionConfig->api_version = value;
				}
			}

			property array<Byte>^ ApplicationKey {
				array<Byte>^ get(){
					if (sessionConfig->application_key != nullptr){
						array<Byte>^ managed = gcnew array<Byte>(sessionConfig->application_key_size);
						Marshal::Copy(IntPtr((void*)sessionConfig->application_key), managed, 0, managed->Length);

						return managed;
					}
					
					return nullptr;
				}
				void set(array<Byte>^ value){
					pin_ptr<System::Byte> p = &value[0];
					sessionConfig->application_key = reinterpret_cast<const char*>((const char*)p);
					sessionConfig->application_key_size = value->Length;
				}
			}

			property String^ CacheLocation {
				String^ get(){
					return gcnew String((const char*)sessionConfig->cache_location);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					sessionConfig->cache_location = static_cast<const char*>(p.ToPointer());
				}
			}

			property String^ SettingsLocation {
				String^ get(){
					return gcnew String((const char*)sessionConfig->settings_location);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					sessionConfig->settings_location = static_cast<const char*>(p.ToPointer());
				}
			}
						
			property String^ UserAgent {
				String^ get(){
					return gcnew String((const char*)sessionConfig->user_agent);
				}
				void set(String^ value){
					IntPtr p = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
					sessionConfig->user_agent = static_cast<const char*>(p.ToPointer());
				}
			}
			
            //public IntPtr callbacks;					
            //public IntPtr userdata;
				
			property bool CompressPlaylists {
				bool get(){
					return sessionConfig->compress_playlists;
				}
				void set(bool value){
					sessionConfig->compress_playlists = value;
				}
			}

			property bool DontSaveMetadataForPlaylists {
				bool get(){
					return sessionConfig->dont_save_metadata_for_playlists;
				}
				void set(bool value){
					sessionConfig->dont_save_metadata_for_playlists = value;
				}
			}

			property bool InitiallyUnloadPlaylists {
				bool get(){
					return sessionConfig->initially_unload_playlists;
				}
				void set(bool value){
					sessionConfig->initially_unload_playlists = value;
				}
			}
		};
	}
}