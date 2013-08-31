using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.SpotifyShell {
	public class ShellListener : ConsoleListener {
		private SessionController sessionController;
		private string username;
		
		protected override void onCommand(string command, params string[] args) {
			switch (command.ToLower()) {
				case "start":
					sessionController.createSession();
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

				case "logout":
					sessionController.logout();
					break;

				case "stop":
					sessionController.stopSession();
					break;

			}
		}

		protected override void onPasswordEntered(string password) {
			sessionController.createSession();
			sessionController.login(username, password);
		}

		protected override void onStart(params string[] args) {
			sessionController = new SessionController();
		}

		protected override void onExit() {
			sessionController.logout();
			sessionController.stopSession();
		}
	}
}
