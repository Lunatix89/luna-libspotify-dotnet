using Luna.LibSpotify;
using Luna.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DemoPlayer.Playback {
	public class DummyPlaybackSession : Session {
		private double totalTrackLength;
		private double consumedTrackLength;
		private double playbackPosition;

		public override double PlaybackPosition {
			get { return playbackPosition; }
			set {
				playbackPosition = value;
			}
		}

		public DummyPlaybackSession(string appkey, string userAgent) {
			SessionConfig.ApplicationKey = Encoding.ASCII.GetBytes(appkey);
			SessionConfig.UserAgent = userAgent;
		}

		public DummyPlaybackSession(byte[] appkey, string userAgent) {
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

		public override void playTrack(Track track) {
			base.playTrack(track);

			PlaybackPosition = 0;
			consumedTrackLength = 0;
			totalTrackLength = (int)track.Duration.TotalMilliseconds;
		}

		public override int onMusicDelivery(AudioFormat audioFormat, short[] pcmData, int numFrames) {
			consumedTrackLength += (numFrames / (audioFormat.SampleRate / 1000.0));
			if (totalTrackLength > 0) {
				PlaybackPosition = consumedTrackLength / totalTrackLength;
			} else {
				PlaybackPosition = 0;
			}

			return numFrames;
		}

	}
}
