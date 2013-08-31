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
			sp_offline_sync_status* unmanagedPointer;

		public:
			property UInt64 CopiedBytes {
				UInt64 get(){
					return unmanagedPointer->copied_bytes;
				}
				void set(UInt64 value){
					unmanagedPointer->copied_bytes = value;
				}
			}
			
			property int CopiedTracks {
				int get(){
					return unmanagedPointer->copied_tracks;
				}
				void set(int value){
					unmanagedPointer->copied_tracks = value;
				}
			}

			property UInt64 DoneBytes {
				UInt64 get(){
					return unmanagedPointer->done_bytes;
				}
				void set(UInt64 value){
					unmanagedPointer->done_bytes = value;
				}
			}

			property int DoneTracks {
				int get(){
					return unmanagedPointer->done_tracks;
				}
				void set(int value){
					unmanagedPointer->done_tracks = value;
				}
			}

			property int ErrorTracks {
				int get(){
					return unmanagedPointer->error_tracks;
				}
				void set(int value){
					unmanagedPointer->error_tracks = value;
				}
			}

			property int QueuedTracks {
				int get(){
					return unmanagedPointer->queued_tracks;
				}
				void set(int value){
					unmanagedPointer->queued_tracks = value;
				}
			}

			property UInt64 QueuedBytes{
				UInt64 get(){
					return unmanagedPointer->queued_bytes;
				}
				void set(UInt64 value){
					unmanagedPointer->queued_bytes = value;
				}
			}
			
			property bool IsSyncing {
				bool get(){
					return unmanagedPointer->syncing;
				}
				void set(bool value){
					unmanagedPointer->syncing = value;
				}
			}

			property int WillNotCopyTracks {
				int get(){
					return unmanagedPointer->willnotcopy_tracks;
				}
				void set(int value){
					unmanagedPointer->willnotcopy_tracks = value;
				}
			}
		};
  
	}
}