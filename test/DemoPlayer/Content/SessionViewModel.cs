using Luna.LibSpotify;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using Luna.Utilities;
using System.Timers;
using System.Windows;

namespace DemoPlayer.Content {
	public class SessionViewModel : INotifyPropertyChanged {
		private Session session;
		private string username;
		private Dispatcher dispatcher;
		private ObservableCollection<LazyTrack> selectedPlaylist;
		private Timer playbackTimer;
		private double actualTrackProgress;
		private TimeSpan trackPosition;
		private Visibility loginVisible = Visibility.Visible;

		public Visibility LoginVisible {
			get { return loginVisible; }
			set {
				loginVisible = value;
				PropertyChanged.Raise(() => LoginVisible);
			}
		}

		public Session Session {
			get { return session; }
			set {
				if (session != null) {
					session.LoggedIn -= session_LoggedIn;
					session.LoggedOut -= session_LoggedOut;
				}
				session = value;

				if (session != null) {
					session.LoggedIn += session_LoggedIn;
					session.LoggedOut += session_LoggedOut;
				}

				PropertyChanged.Raise(() => Session);
			}
		}

		void session_LoggedOut(object sender, EventArgs e) {
			LoginVisible = Visibility.Collapsed;
		}

		private void session_LoggedIn(object sender, SpErrorCode e) {
			if (e == SpErrorCode.Ok) {
				LoginVisible = Visibility.Collapsed;
				UserName = Session.ActiveUser.CanonicalName;
				Playlist playlist = Session.createStarred();

				if (playlist != null) {
					dispatcher.Invoke(() => {
						SelectedPlaylist = new ObservableCollection<LazyTrack>();
						int count = playlist.NumTracks;
						for (int n = 0; n < count; n++) {
							SelectedPlaylist.Add(new LazyTrack(playlist, n));
						}
					});
				}
			} else {
				MessageBox.Show("Could not log in: " + e);
			}
		}

		public string UserName {
			get { return username; }
			set {
				username = value;
				PropertyChanged.Raise(() => UserName);
			}
		}

		public TimeSpan TrackPosition {
			get { return trackPosition; }
			set {
				if (value != trackPosition) {
					trackPosition = value;
					PropertyChanged.Raise(() => TrackPosition);
				}
			}
		}

		public double ActualTrackProgress {
			get { return actualTrackProgress; }
			set {
				if (value != actualTrackProgress) {
					actualTrackProgress = value;
					TrackPosition = TimeSpan.FromMilliseconds(session.ActualTrack.Duration.TotalMilliseconds * value);

					PropertyChanged.Raise(() => ActualTrackProgress);
				}
			}
		}

		public ObservableCollection<LazyTrack> SelectedPlaylist {
			get { return selectedPlaylist; }
			set {
				selectedPlaylist = value;
				PropertyChanged.Raise(() => SelectedPlaylist);
			}
		}

		public SessionViewModel() {
			dispatcher = Dispatcher.CurrentDispatcher;
			playbackTimer = new Timer(25);
			playbackTimer.Elapsed += (s, e) => {
				ActualTrackProgress = session.PlaybackPosition;
			};
			playbackTimer.Start();
		}

		public void playTrack(LazyTrack lazyTrack) {
			session.playTrack(lazyTrack.Track);
		}

		#region INotifyPropertyChanged Members

		public event PropertyChangedEventHandler PropertyChanged;

		#endregion
	}
}
