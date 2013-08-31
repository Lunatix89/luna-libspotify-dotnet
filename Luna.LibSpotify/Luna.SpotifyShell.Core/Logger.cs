using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public abstract class Logger {
		private static Logger instance;
		private LogLevel logLevel = LogLevel.All;
		private bool showStackTrace = true;



		public LogLevel LogLevel {
			get { return logLevel; }
			set { logLevel = value; }
		}

		public bool ShowStackTrace {
			get { return showStackTrace; }
			set { showStackTrace = value; }
		}

		/// <summary>
		/// Gets or creates an instance of a Logger class
		/// </summary>
		public static Logger Instance {
			get {
				if (instance == null) {
					throw new NullReferenceException("Logger has to be initialized by the executing application");
				}
				return instance;
			}
			set {
				instance = value;
			}
		}

		public Logger() {

		}

		/// <summary>
		/// Logs a message (Debug: in debug output window, Release: in console, both: in file, when available)
		/// </summary>
		/// <param name="message">The message to log</param>
		public void log(string message, params object[] args) {
			if (logLevel == LogLevel.All) {
				string msg = String.Format(message, args);
				log(msg, true, true);
			}
		}

		/// <summary>
		/// Logs a custom exception message (Debug: in debug output window, Release: in console, both: in file, when available)
		/// </summary>
		/// <param name="message">The message to log</param>
		public void logError(string message, string stackTrace, params object[] args) {
			logError(MessageType.Normal, message, stackTrace, args);
		}

		/// <summary>
		/// Logs a custom exception message (Debug: in debug output window, Release: in console, both: in file, when available)
		/// </summary>
		/// <param name="message">The message to log</param>
		public void logError(MessageType level, string message, string stackTrace, params object[] args) {
			if ((int)level >= (int)logLevel) {
				string msg = String.Format("[{0}] {1}", level, String.Format(message, args));

				if ((ShowStackTrace) && (!String.IsNullOrEmpty(stackTrace))) {
					log(String.Format("{0}\n{1}", msg, stackTrace), true, true);
				} else {
					log(msg, true, true);
				}
			}
		}

		/// <summary>
		/// Logs an exception and its stacktrace (Debug: in debug output window, Release: in console, both: in file, when available)
		/// </summary>
		/// <param name="message">The message to log</param>
		public void log(Exception ex) {
			log(String.Format("{0}{1}{2}", ex.Message, Environment.NewLine, ex.StackTrace), true, true, true);
		}

		protected abstract void log(string message, bool inConsole, bool inFile, bool isException = false);

	}
}
