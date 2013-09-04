using Luna.LibSpotify;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DemoPlayer.Content {
	public class LazyTrack {
		private static readonly Track dummyTrack = Track.createLocal("N/A", "N/A", "N/A", 0);
		private Track track;
		private Playlist playlist;
		private int index;

		public int Index {
			get { return index; }
		}

		public Track Track {
			get {
				if (track == null) {
					track = playlist.getTrack(index);
				}
				return track ?? dummyTrack;
			}
		}

		public string Title {
			get { return Track.Name; }
		}

		public string Artist {
			get { return Track.getArtist(0).Name; }
		}

		public string Album {
			get { return Track.Album.Name; }
		}

		public LazyTrack(Playlist playlist, int trackIndex) {
			this.playlist = playlist;
			this.index = trackIndex;
		}

	}
}
