#include "SessionConfig.h"
#include <stdint.h>
#include <memory>
#include "Session.h";



using namespace System;
using namespace System::Collections::Generic;

namespace Luna {
	namespace LibSpotify {

		static void SP_CALLCONV notify_main_thread(sp_session *sess) {
			Session::notifyMainThread(sess);
		}

		/**
		 * This callback is called when the user was logged in, but the connection to
		 * Spotify was dropped for some reason.
		 *
		 * @sa sp_session_callbacks#connection_error
		 */
		static void SP_CALLCONV connection_error(sp_session *session, sp_error error) {

		}

		/**
		 * This callback is called when an attempt to login has succeeded or failed.
		 *
		 * @sa sp_session_callbacks#logged_in
		 */
		static void SP_CALLCONV logged_in(sp_session *session, sp_error error) {
			Session::raiseLoggedIn(session, error);
		}

		/**
		 * This callback is called when the session has logged out of Spotify.
		 *
		 * @sa sp_session_callbacks#logged_out
		 */
		static void SP_CALLCONV logged_out(sp_session *session) {
			Session::raiseLoggedOut(session);
		}

		/**
		 * This callback is called when there is a scrobble error.
		 *
		 * @sa sp_session_callbacks#scrobble_error
		 */
		static void SP_CALLCONV scrobble_error(sp_session* session, sp_error error) {

		}

		/**
		 * Called when there is a change in the private session mode
		 *
		 * @param[in]  session    Session
		 * @param[in]  isPrivate  True if in private session, false otherwhise
		 */
		static void SP_CALLCONV private_session_mode_changed(sp_session *session, bool is_private) {

		}

		/**
		 * This callback is called when the session have recieved a credential
		 * that could be stored safely on disk
		 *
		 * @sa sp_session_callbacks#credentials_blob_updated
		 */
		static void SP_CALLCONV credentials_blob_updated(sp_session *session, const char *blob) {

		}

		/**
		 * This callback is called for log messages.
		 *
		 * @sa sp_session_callbacks#log_message
		 */
		static void SP_CALLCONV log_message(sp_session *session, const char *data) {
			Session::raiseLogMessage(session, data);
		}

		/**
		 * Callback called when libspotify has new metadata available
		 *
		 * Not used in this example (but available to be able to reuse the session.c file
		 * for other examples.)
		 *
		 * @sa sp_session_callbacks#metadata_updated
		 */
		static void SP_CALLCONV metadata_updated(sp_session *sess) {

		}

		/**
		 *
		 */
		static void SP_CALLCONV offline_status_updated(sp_session *sess) {

		}

		static int SP_CALLCONV music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames) {
			return Session::raiseOnMusicDelivery(session, format, frames, num_frames);
		}

		static void SP_CALLCONV play_token_lost(sp_session *session) {
			Session::raisePlayTokenLost(session);
		}

		static void SP_CALLCONV end_of_track(sp_session *session) {
			Session::raiseEndOfTrack(session);
		}


		SessionConfig::SessionConfig(sp_session_config* unmanagedPointer){
			this->unmanagedPointer = unmanagedPointer;
			init();
		}

		SessionConfig::SessionConfig(){
			unmanagedPointer = new sp_session_config();
			init();
		}

		SessionConfig::~SessionConfig(){
			if (unmanagedPointer != nullptr){
				delete unmanagedPointer;
			}
			if (callbacks != nullptr){
				delete callbacks;
			}
		}

		void SessionConfig::init(){
			callbacks = new sp_session_callbacks();
			memset(callbacks, 0, sizeof(sp_session_callbacks));
			memset(unmanagedPointer, 0, sizeof(sp_session_config));

			// Register the callbacks.
			callbacks->logged_in = logged_in;
			callbacks->logged_out = logged_out;
			callbacks->metadata_updated = metadata_updated;
			callbacks->connection_error = connection_error;
			callbacks->notify_main_thread = notify_main_thread;
		
			callbacks->music_delivery = music_delivery;
			callbacks->play_token_lost = play_token_lost;
			callbacks->end_of_track = end_of_track;
		
			callbacks->log_message = log_message;
			callbacks->offline_status_updated = offline_status_updated;
			callbacks->credentials_blob_updated = credentials_blob_updated;
			callbacks->scrobble_error = scrobble_error;
			callbacks->private_session_mode_changed = private_session_mode_changed;

			unmanagedPointer->api_version = SPOTIFY_API_VERSION;
			unmanagedPointer->cache_location = "tmp";
			unmanagedPointer->settings_location = "tmp";
			unmanagedPointer->callbacks = callbacks;
			unmanagedPointer->proxy = "";
		}

	}
}