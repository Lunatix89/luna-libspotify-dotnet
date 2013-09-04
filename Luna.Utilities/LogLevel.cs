using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.Utilities {
	public enum LogLevel {
		/// <summary>
		/// All log messages will be visible
		/// </summary>
		All = 0,

		/// <summary>
		/// Only errors will be visible, information messages will be suppressed
		/// </summary>
		Error = 1,

		/// <summary>
		/// Only fatal errors will be visible, information and normal errors will be suppressed
		/// </summary>
		FatalOnly = 2,

		/// <summary>
		/// All messages will be suppressed
		/// </summary>
		Off = 9999,
	}
}
