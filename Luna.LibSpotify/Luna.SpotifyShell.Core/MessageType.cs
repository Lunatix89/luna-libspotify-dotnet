using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public enum MessageType {
		/// <summary>
		/// Information message
		/// </summary>
		Information = 0,

		/// <summary>
		/// Normal error
		/// </summary>
		Normal = 1,

		/// <summary>
		/// Fatal error
		/// </summary>
		Fatal = 2,
	}
}
