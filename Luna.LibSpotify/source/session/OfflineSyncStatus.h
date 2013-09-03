#pragma once
#include "../../include/libspotify/api.h"
#include "../Enumerations.h"
#include <stdint.h>

using namespace System;
using namespace System::Runtime::InteropServices;


namespace Luna {
	namespace LibSpotify {
		public ref struct OfflineSyncStatus {
		internal:
			sp_offline_sync_status* audioformat;

		public:
			property UInt64 CopiedBytes {
				UInt64 get(){
					return audioformat->copied_bytes;
				}
				void set(UInt64 value){
					audioformat->copied_bytes = value;
				}
			}
			
			property int CopiedTracks {
				int get(){
					return audioformat->copied_tracks;
				}
				void set(int value){
					audioformat->copied_tracks = value;
				}
			}

			property UInt64 DoneBytes {
				UInt64 get(){
					return audioformat->done_bytes;
				}
				void set(UInt64 value){
					audioformat->done_bytes = value;
				}
			}

			property int DoneTracks {
				int get(){
					return audioformat->done_tracks;
				}
				void set(int value){
					audioformat->done_tracks = value;
				}
			}

			property int ErrorTracks {
				int get(){
					return audioformat->error_tracks;
				}
				void set(int value){
					audioformat->error_tracks = value;
				}
			}

			property int QueuedTracks {
				int get(){
					return audioformat->queued_tracks;
				}
				void set(int value){
					audioformat->queued_tracks = value;
				}
			}

			property UInt64 QueuedBytes{
				UInt64 get(){
					return audioformat->queued_bytes;
				}
				void set(UInt64 value){
					audioformat->queued_bytes = value;
				}
			}
			
			property bool IsSyncing {
				bool get(){
					return audioformat->syncing;
				}
				void set(bool value){
					audioformat->syncing = value;
				}
			}

			property int WillNotCopyTracks {
				int get(){
					return audioformat->willnotcopy_tracks;
				}
				void set(int value){
					audioformat->willnotcopy_tracks = value;
				}
			}
		};
  
	}
}