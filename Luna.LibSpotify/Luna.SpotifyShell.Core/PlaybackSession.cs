using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Luna.LibSpotify;
using System.IO;

namespace Luna.SpotifyShell {
	public class PlaybackSession  : Session {
		private readonly object streamLock = new object();
		private Stream playbackStream;
		private BinaryWriter playbackWriter;


		public Stream PlaybackStream {
			get { return playbackStream; }
			set {
				lock (streamLock) {
					playbackStream = value;
					playbackWriter = new BinaryWriter(playbackStream, Encoding.Default, true);
				}
			}
		}

		public PlaybackSession(string appkey, string userAgent) {
			SessionConfig.ApplicationKey = Encoding.ASCII.GetBytes(appkey);
			SessionConfig.UserAgent = userAgent;
		}

		public PlaybackSession(byte[] appkey, string userAgent) {
			SessionConfig.ApplicationKey = appkey;
			SessionConfig.UserAgent = userAgent;
		}

		public override void onSessionCreated(SpErrorCode errorcode) {
			base.onSessionCreated(errorcode);

			if (errorcode == SpErrorCode.Ok) {
				Logger.Instance.log("Spotify session successfully created");
			} else {
				Logger.Instance.log("Could not create spotify session: {0}", errorcode);
			}
		}

		public override void onLoggedIn(SpErrorCode errorcode) {
			base.onLoggedIn(errorcode);

			if (errorcode == SpErrorCode.Ok) {
				Logger.Instance.log("User successfully logged in.");
			} else {
				Logger.Instance.log("User failed to log in: {0}.", errorcode);
			}
		}

		public override void onEndOfTrack() {
			base.onEndOfTrack();
		}

		public override void onLoggedOut() {
			base.onLoggedOut();

			Logger.Instance.log("User logged out.");
		}

		public override void onSessionReleased() {
			base.onSessionReleased();

			Logger.Instance.log("Spotify session released");
		}

		public override void onLogMessage(string message) {
			base.onLogMessage(message);

			Logger.Instance.log(message);
		}

		public override int onMusicDelivery(AudioFormat audioFormat, short[] pcmData, int numFrames) {
			if (playbackStream != null) {
				lock (streamLock) {
					for (int n = 0; n < pcmData.Length; n++) {
						playbackWriter.Write(pcmData[n]);
					}

				}
			}

			return numFrames;
		}
	}
}
