using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.Utilities {
	public class FileLogger : ConsoleLogger {
		private StreamWriter streamWriter;
		private string logDirectory;

		public string LogDirectory {
			get { return logDirectory; }
			private set { logDirectory = value; }
		}

		public FileLogger(string logDirectory) {
			this.logDirectory = logDirectory;
		}

		/// <summary>
		/// Tries to initialize a StreamWriter on a log file
		/// </summary>
		private void checkStreamWriter() {
			if (streamWriter == null) {
				try {
					if (!Directory.Exists(logDirectory)) {
						Directory.CreateDirectory(logDirectory);
					}

					string filename = String.Format("{0}aip_{1:dd_MM_yyyy_HH_mm_ss}.log", logDirectory.Replace("/", "\\"), DateTime.Now);
					streamWriter = new StreamWriter(File.Create(filename));
				} catch {

				}
			}
		}

		public override void writeToFile(string message) {
			checkStreamWriter();

			if (streamWriter != null) {
				streamWriter.Write(message);
				streamWriter.Flush();
			}
		}

	}
}
