#pragma once
#include "../include/libspotify/api.h"


namespace Luna {
	namespace LibSpotify {
		public enum class SpErrorCode {
			ApiInitializationFailed = sp_error::SP_ERROR_API_INITIALIZATION_FAILED,
			ApplicationBanned = sp_error::SP_ERROR_APPLICATION_BANNED,
			BadApiVersion = sp_error::SP_ERROR_BAD_API_VERSION,
			BadApplicationKey = sp_error::SP_ERROR_BAD_APPLICATION_KEY,
			BadUsernameOrPassword = sp_error::SP_ERROR_BAD_USERNAME_OR_PASSWORD,
			BadUserAgent = sp_error::SP_ERROR_BAD_USER_AGENT,
			CantOpenTraceFile = sp_error::SP_ERROR_CANT_OPEN_TRACE_FILE,
			ClientTooOld = sp_error::SP_ERROR_CLIENT_TOO_OLD,
			InboxIsFull = sp_error::SP_ERROR_INBOX_IS_FULL,
			InvalidArgument = sp_error::SP_ERROR_INVALID_ARGUMENT,
			InvalidDeviceId = sp_error::SP_ERROR_INVALID_DEVICE_ID,
			InvalidInData = sp_error::SP_ERROR_INVALID_INDATA,
			IsLoading = sp_error::SP_ERROR_IS_LOADING,
			LastFMAuthError = sp_error::SP_ERROR_LASTFM_AUTH_ERROR,
			MissingCallback = sp_error::SP_ERROR_MISSING_CALLBACK,
			NetworkDisabled = sp_error::SP_ERROR_NETWORK_DISABLED,
			NoCache = sp_error::SP_ERROR_NO_CACHE,
			NoCredentials = sp_error::SP_ERROR_NO_CREDENTIALS,
			NoStreamAvailable = sp_error::SP_ERROR_NO_STREAM_AVAILABLE,
			NoSuchUser = sp_error::SP_ERROR_NO_SUCH_USER,
			OfflineDiskCache = sp_error::SP_ERROR_OFFLINE_DISK_CACHE,
			OfflineExpired = sp_error::SP_ERROR_OFFLINE_EXPIRED,
			OfflineLicenseError = sp_error::SP_ERROR_OFFLINE_LICENSE_ERROR,
			OfflineNotAllowed = sp_error::SP_ERROR_OFFLINE_NOT_ALLOWED,
			OfflineTooManyTracks = sp_error::SP_ERROR_OFFLINE_TOO_MANY_TRACKS,
			Ok = sp_error::SP_ERROR_OK,
			OtherPermanent = sp_error::SP_ERROR_OTHER_PERMANENT,
			OtherTransient = sp_error::SP_ERROR_OTHER_TRANSIENT,
			PermissionDenied = sp_error::SP_ERROR_PERMISSION_DENIED,
			SystemFailure = sp_error::SP_ERROR_SYSTEM_FAILURE,
			TrackNotAvailable = sp_error::SP_ERROR_TRACK_NOT_PLAYABLE,
			UnableToContactServer = sp_error::SP_ERROR_UNABLE_TO_CONTACT_SERVER,
			UserBanned = sp_error::SP_ERROR_USER_BANNED,
			UserNeedsPremium = sp_error::SP_ERROR_USER_NEEDS_PREMIUM,
		};

		public enum class ConnectionState {
            LoggedOut = 0,
            LoggedIn = 1,
            Disconnected = 2,
            Undefined = 3,
            Offline = 4
        };

        public enum class SampleType {
            Int16NativeEndian = 0
        }; 

		public enum class Bitrate {
			Bitrate160k = 0,
			Bitrate320k = 1,
			Bitrate96k = 2,
		};

		public enum class PlaylistType {
			Playlist = 0,
			StartFolder = 1,
			EndFolder = 2,
			PlaceHolder = 3,
		};

		public enum class PlaylistOfflineStatus {
			No = 0,
			Yes = 1,
			Downloading = 2,
			Waiting = 3,
		};
			
		public enum class Availability {
			Unavailable = 0,
			Available = 1,
			NotStreamable = 2,
			BannedByArtist = 3,
		};

		public enum class TrackOfflineStatus {
			No = 0,
			Waiting = 1,
			Downloading = 2,
			Done = 3,
			Error = 4,
			DoneExpired = 5,
			LimitExceeded = 6,
			DoneResync = 7,
		};
		
		public enum ImageSize {
			Normal = 0,
			Small = 1,
			Large = 2,
		};

		public enum class ConnectionType {
			Unknown = 0,
			None = 1,
			Mobile = 2,
			MobileRoaming = 3,
			WiFi = 4,
			Wired = 5,
		};

		public enum class ConnectionRules {
			Network = 0x1,
			NetworkIfRoaming = 0x2,
			AllowSyncOverMobile = 0x4,
			AllowSyncOVerWiFi = 0x8,
		};

		public enum class SocialProvider {
			Spotify = 0,
			Facebook = 1,
			LastFM = 2,
		};

		public enum class ScrobblingState {
			UseGlobalSetting = 0,
			LocalEnabled = 1,
			LocalDisabled = 2,
			GlobalEnabled = 3,
			GlobalDisabled = 4,
		};
	}
}