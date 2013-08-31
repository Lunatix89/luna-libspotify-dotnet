using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	class Program {
		static void Main(string[] args) {
			Logger.Instance = new ConsoleLogger();
			new ShellListener().run();
		}
	}
}
