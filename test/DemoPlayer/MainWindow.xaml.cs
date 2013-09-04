using Luna.LibSpotify;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Luna.Utilities;
using System.IO;
using System.Windows.Threading;
using DemoPlayer.Content;
using DemoPlayer.Playback;

namespace DemoPlayer {
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	/// 


	

	public partial class MainWindow : Window {
		private SessionViewModel sessionViewModel;

		public MainWindow() {
			Logger.Instance = new ConsoleLogger();
			InitializeComponent();

			Loaded += MainWindow_Loaded;
			Closing += MainWindow_Closing;
		}

		void MainWindow_Closing(object sender, CancelEventArgs e) {
			if (sessionViewModel.Session.IsLoggedIn) {
				sessionViewModel.Session.logout();
			}
			if (sessionViewModel.Session.IsSessionCreated) {
				sessionViewModel.Session.releaseSession();
			}
		}

		void MainWindow_Loaded(object sender, RoutedEventArgs e) {
			byte[] appkey = File.ReadAllBytes("spotify_appkey.key");

			sessionViewModel = new SessionViewModel();
			sessionViewModel.Session = new DummyPlaybackSession(appkey, "DemoPlayer");
			sessionViewModel.Session.createSession();

			DataContext = sessionViewModel;
		}

		private void ListView_MouseDoubleClick(object sender, MouseButtonEventArgs e) {
			DependencyObject dp = (DependencyObject)e.OriginalSource;
			while ((dp != null) && (dp != (ListView)sender)) {
				if (dp is ListViewItem) {
					if (dp is FrameworkElement) {
						FrameworkElement frameworkElement = (FrameworkElement)dp;
						if (frameworkElement.DataContext is LazyTrack) {
							sessionViewModel.playTrack((LazyTrack)frameworkElement.DataContext);
						}
					}
					break;
				}
				dp = VisualTreeHelper.GetParent(dp);
			}
		}

		private void Button_Click(object sender, RoutedEventArgs e) {
			string username = userField.Text;
			string password = passwordField.Password;

			sessionViewModel.Session.login(username, password);
		}
	}
}
