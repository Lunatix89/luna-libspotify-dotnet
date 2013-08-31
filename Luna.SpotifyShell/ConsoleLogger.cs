using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public class ConsoleLogger : Logger {

		public ConsoleLogger() {

			
		}

		public virtual void writeToFile(string message) {

		}

		protected override void log(string message, bool inConsole, bool inFile, bool isException = false) {
			message = String.Format("[{0:HH:mm:ss}] {1}{2}", DateTime.Now, message, Environment.NewLine);

			if (inFile) {
				writeToFile(message);
			}

			if (inConsole) {
#if DEBUG
				System.Diagnostics.Debug.Write(message);
#endif
				Console.Write(message.Replace("\r\n", "\n"));
			}
		}
	}
}
