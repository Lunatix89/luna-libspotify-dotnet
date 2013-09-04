using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Luna.Utilities {
	public abstract class ConsoleListener {

		public bool IsInPasswordMode {
			get;
			protected set;
		}

		/// <summary>
		/// Runs the console listener
		/// </summary>
		/// <param name="arguments">Optional arguments to pass</param>
		public void run(params string[] arguments) {
			onStart(arguments);

			string password = "";
			bool exit = false;
			do {
				if (IsInPasswordMode) {
					ConsoleKeyInfo keyInfo = Console.ReadKey();

					if (keyInfo.Key == ConsoleKey.Enter) {
						IsInPasswordMode = false;
						onPasswordEntered(password);
						password = "";
					} else if (keyInfo.Key == ConsoleKey.Backspace) {
						if (password.Length > 0) {
							password = password.Substring(0, password.Length - 1);
						}
					} else {
						Console.Write("\b \b");
						password += keyInfo.KeyChar;
					}
				} else {
					string commandLine = Console.ReadLine();
					string[] split = commandLine.Split(' ');

					if (split.Length > 0) {
						switch (split[0].ToLower()) {
							case "exit":
								exit = true;
								break;

							default:
								string[] args = null;
								if (split.Length > 1) {
									args = new string[split.Length - 1];
									Array.Copy(split, 1, args, 0, args.Length);
								}

								onCommand(split[0], args);
								break;

						}
					}
				}


			} while (!exit);

			onExit();
		}

		/// <summary>
		/// Outputs a message with formatting (see String.Format)
		/// </summary>
		/// <param name="format">The format</param>
		/// <param name="args">Optional arguments for formatting</param>
		protected void print(string format, params object[] args) {
			string message = String.Format(format, args);
			print(message);
		}

		/// <summary>
		/// Outputs a message
		/// </summary>
		/// <param name="message">The message to print</param>
		protected void print(string message) {
			if (Logger.Instance != null) {
				Logger.Instance.log(message);
			} else {
				Console.Write(message);
			}
		}

		/// <summary>
		/// Called, when a command was entered
		/// </summary>
		/// <param name="command">The command</param>
		/// <param name="args">Optional arguments</param>
		protected abstract void onCommand(string command, params string[] args);

		/// <summary>
		/// Called, when a password was entered
		/// </summary>
		/// <param name="password">The typed password</param>
		protected abstract void onPasswordEntered(string password);

		/// <summary>
		/// Called, when the ConsoleListener is about to be starting
		/// </summary>
		/// <param name="args">Optional arguments which where passed by the executing instance</param>
		protected abstract void onStart(params string[] args);

		/// <summary>
		/// Called, when the ConsoleListeners loop was exited
		/// </summary>
		protected abstract void onExit();
	}
}
