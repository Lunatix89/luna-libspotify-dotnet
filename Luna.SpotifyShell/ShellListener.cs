using Luna.LibSpotify;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public class ShellListener : ConsoleListener {
		private PlaybackSession session;
		private Track track;
		private string username;
		
		protected override void onCommand(string command, params string[] args) {
			switch (command.ToLower()) {
				case "start":
					session.createSession();
					break;

				case "login":
					if ((args != null) && (args.Length > 0)) {
						username = args[0];

						print("password: ");
						IsInPasswordMode = true;
					} else {
						print("Please enter a user to log in");
					}

					break;

				case "set":
					if ((args != null) && (args.Length > 0)) {
						onSet(args);
					} else {
						print("Incomplete set command.");
					}
					break;

				case "playback":
					if ((args != null) && (args.Length > 0)) {
						onPlayback(args);
					} else {
						print("Incomplete playback command.");
					}
					break;

				case "logout":
					session.logout();
					break;

				case "stop":
					session.releaseSession();
					break;

			}
		}

		private string getArgumentValue(string[] args, string term) {
			int index = Array.IndexOf(args, 
										(from arg in args
										where arg == term
										select term).FirstOrDefault());


			return ((index > 0) & (index+1 < args.Length) ? args[index + 1] : "");
		}

		private void onPlayback(string[] args) {
			if (args.Count() > 0) {
				try {
					switch (args[0]) {
						case "load":
							session.playerLoad(track);
							break;

						case "play": {
								FileStream fs = new FileStream(String.Format("{0} - {1}.wav", track.Index, track.Name), FileMode.Create, FileAccess.Write);
								byte[] header = new byte[44];
								fs.Write(header, 0, header.Length);

								session.PlaybackStream = fs;
								session.playerPlay(true);
							} break;

						case "stop": {
								session.playerPlay(false);
								byte[] header = RiffHeader.create((uint)session.PlaybackStream.Length - 44, (uint)44100, (uint)2, (uint)16);

								session.PlaybackStream.Position = 0;
								session.PlaybackStream.Write(header, 0, header.Length);
								session.PlaybackStream.Close();
								session.PlaybackStream = null;

							} break;

						case "prefetch":
							session.playerPrefetch(track);
							break;

						case "unload":
							session.playerUnload();
							break;

					}
				} catch (Exception ex) {
					Logger.Instance.logError("Could not evaluate command: {0}", ex.StackTrace, ex.Message);
				}
			} else {
				print("Incomplete set command.");
			}
		}

		private void onSet(string[] args) {
			if (args.Count() > 0) {
				switch (args[0]) {

					case "track":
						if (args.Length < 2) {
							Link link = Link.createFromString("spotify:track:28q2v77HT5TrNO4Spm4AYk");
							track = link.asTrack();
						} else {
							Link link = Link.createFromString(args[2]);
							track = link.asTrack();

							/*
							string title = getArgumentValue(args, "-t");
							string album = getArgumentValue(args, "-al");
							string artist = getArgumentValue(args, "-at");

							track = Track.createLocal(artist, album, title, 0);*/
						}

						print("localtrack: {0}", (track != null ? track.Name : "null"));

						break;

					case "playable":
						if (track != null) {
							if (track.isPlayable(session)) {
								print("the given track is playable.");
							} else {
								print("the given track is not playable.");
							}
						} else {
							print("no local track specified.");
						}

						break;

					

					default:
						print("Unknown set option: {0}", args[0]);
						break;

				}
			} else {
				print("Incomplete set command.");
			}
		}

		protected override void onPasswordEntered(string password) {
			session.createSession();
			session.login(username, password);
		}

		protected override void onStart(params string[] args) {
			byte[] appkey = File.ReadAllBytes("spotify_appkey.key");

			session = new PlaybackSession(appkey, "SpotifyDotnetTest");
		}

		protected override void onExit() {
			session.logout();
			session.releaseSession();
		}
	}
}
