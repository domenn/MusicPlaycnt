#include "do_things.hpp"

#include <src/data/accessor.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/model/song_with_metadata.hpp>

using namespace msw::model;

namespace msw::do_things {

class Handler {
 private:
  SongWithMetadata new_song_;
  ActionType new_song_action_type_{new_song_.action_type()};

 public:
  Handler(SongWithMetadata&& new_song);
  void update_data_for_currently_stored_song();
  void advance();
};

Handler::Handler(SongWithMetadata&& new_song) : new_song_(std::move(new_song)) {
  // TODO app_cfg ...
}

void Handler::update_data_for_currently_stored_song() {
  switch (new_song_action_type_) {
    case model::ActionType::NONE:
      return;
    case ActionType::STOP:
    case ActionType::PAUSE:
    default:
      return;
  }
  // noop ... todo implement.
}

void Handler::advance() { pg::handled_song->replace(&new_song_); }

void new_song_happened(msw::model::SongWithMetadata&& new_song) {
  Handler handler(std::move(new_song));
  handler.update_data_for_currently_stored_song();
  handler.advance();
}
}  // namespace msw::do_things

/*


    # """Scenarios:
    # * 1:: Current song is playing and last song is playing. Conditionally (playtime) increment last song PC.
    # * 2:: Current song is stopped, last song playing. Leave last song same and allow for resume. Set status to
   stopped. Write (append) playtime. # * 3:: Current song is paused, last song playing. Same as stopped. Status to
   paused. Write (append) playtime. # * 4:: Current is not running or bad entry. Basically same as stopped and so. # *
   5:: Current song is playing, Last song is pause or stop or bad, #  *  5.1 if songs are same, save current time and
   set status (to playing) #  .  5.2 If not, set last_song = current_song .. conditionally increment play time
    #
    # Strategy:
    # * For play action set action timestamp.
    # * Once this sw starts, read the log and set initial state. After unpickling thing.
    #
    # Test cases:
    # * Song on repeat, playcount must increase
    # * Do stop and play, on next song, have PC increase
    # * play, close player, open player and play same song. PC must not increase, but increases on next song (PC +1)
    # * play, close player, open player and play DIFFERENT song. PC must increase for previous song
    #
    # """

    def handle_playcount_action(self, song: LoggedSong):
        """
        Does what we want .. returns what is happening
        :param song: song to handle
        :return: true if song was changed
        """
        is_song_change = False
        self.diaglog.debug("handle_playcount_action called. Parameters - song:%s", song)
        last_song: LoggedSong = self.playing_song
        if last_song is None:
            self.diaglog.debug("handling, last song is none, will be handling unknown scenario with %s; %s", song,
                               getattr(song, "playing_status", None))
            self.handle_unknown_scenario(song)
            return True
        if song is None:
            return is_song_change
        if song.is_shutdown_state():
            # We did stop or player shutdown. Last song is therfore still same.
            self.diaglog.debug("handling, Current song set to shutdown state. Stop or similiar happened. Status: %s",
                               song.playing_status)
            song_cpy = song
            song = copy.copy(last_song)
            song.playing_status = song_cpy.playing_status
        self.diaglog.debug("handling, will do main checks. Status: song: %s (%s); last song: %s (%s)", song,
                           getattr(song, "playing_status", None), last_song, getattr(last_song, "playing_status", None))
        if song == last_song and last_song.playing_status == LoggedSong.STATUS_PLAYING and song ==
   LoggedSong.STATUS_PLAYING: self.diaglog.debug( "handling, songs are same. Last song and current song are playing.
   Probably listen on repeat, will add playcount") # scenario 1: repeat self.conditionally_increment_pc(song)
            self.playing_song = song
            self.playback_start_time = datetime.now()
            pass
        elif song == last_song and song.playing_status != LoggedSong.STATUS_PLAYING and last_song.playing_status ==
   LoggedSong.STATUS_PLAYING: # 2:: and 3:: and 4:: We did stop or pause or exit SW. Update status of last song and stop
   timing. self.diaglog.debug("handling, stop or similiar event happened. Last song equals current song %s",
                               self.playing_song)
            last_song.playing_seconds += (datetime.now() - self.playback_start_time).seconds
            last_song.playing_status = song.playing_status
            self.diaglog.debug(
                "handling, last song playing status changed. Last song: %s (%s)\nLast song playtime set to %s.",
                last_song, last_song.playing_status, last_song.playing_seconds)
        elif song == last_song and song.playing_status != LoggedSong.STATUS_PLAYING and last_song.playing_status !=
   LoggedSong.STATUS_PLAYING: self.diaglog.debug( "handling, song went from nonplaying to nonplaying, exacly %s to
   %s\nBasically do nothing, just change last song playing status.", last_song.playing_status, song.playing_status)
            self.playing_song.playing_status = song.playing_status
        elif song == last_song and last_song.playing_status != LoggedSong.STATUS_PLAYING and song.playing_status ==
   LoggedSong.STATUS_PLAYING: # 5.1:: We did resume from pause or stop or smth. Do not increase PC. Set song as playing
   and take previous playtime. self.diaglog.debug("handling, resume from pause / stop / dead. Last song equals current
   song %s", self.playing_song) self.playing_song = song song.playing_seconds = last_song.playing_seconds
            self.playback_start_time = datetime.now()
        elif song != last_song:
            is_song_change = True
            self.diaglog.debug("handling, songchange, song: %s (%s); last song: %s (%s)", song, song.playing_status,
                               last_song, last_song.playing_status)
            # 5.2:: we did switch. Actually I don't care if last song was playing or not. We have to conditionally
   increment PC. # If last song was playing, add to it's play status if last_song.playing_status ==
   LoggedSong.STATUS_PLAYING: pt = (datetime.now() - self.playback_start_time).seconds self.diaglog.debug("handling,
   lastsong was playing. Setting playtime to %s", pt) last_song.playing_seconds += pt
            self.conditionally_increment_pc(last_song)
            self.playing_song = song
            self.diaglog.debug("handling, playing song is now %s", self.playing_song)
            self.playback_start_time = datetime.now()
        else:
            # Unknown scenario. We will probably miss some PC. But no big deal ... Reset all statuses
            self.handle_unknown_scenario(song)
        return is_song_change

    def conditionally_increment_pc(self, song: LoggedSong):
        """ Increment if playing time is high enough"""
        if song.playing_seconds >= self.playcount_threshold_seconds_for_song:
            quer = song.quer_to_get
            try:
                pc = exec_beets("ls -f $playcnt " + quer, eat_exceptions=False, as_string=False)
                if len(pc) != 1:
                    self.diaglog.warning("Trying to modify %s, but query doesn't return correctly. Return length is %d",
                                         quer, len(pc))
                    self.diaglog.debug(pc)
                    return
                try:
                    ipc = int(pc[0])
                except ValueError as ve:
                    if pc[0] != "$playcnt":
                        self.diaglog.warning("Unknown ValueError parsing playcount. Value is: %s", pc[0])
                        self.diaglog.debug(ve, exc_info=True)
                    ipc = 0
                beet_says = exec_beets("modify -y -W " + quer + " playcnt=" + str(ipc + 1), eat_exceptions=False)
                self.diaglog.info(
                    "Set playcount of " + song.get_song_string() + " to " + str(ipc + 1) + "\nBeet says: " + beet_says)
            except Exception as xc:
                self.diaglog.error("Cannot beet modify! Are your plugins configured OK? Exception status in log file.")
                self.diaglog.info(xc, exc_info=True)
        else:
            self.diaglog.debug("Playcount of " + song.get_song_string() + " unchanged due to low play time: " + str(
                song.playing_seconds))

    def handle_unknown_scenario(self, song):
        """If there is a playing song, conditionally set playcount if time is enough"""
        self.diaglog.debug("Unknown - Handling unknown scenario: %s", song)
        if self.playing_song is not None:
            self.diaglog.debug("Unknown - Will try increment. %s", song)
            self.conditionally_increment_pc(self.playing_song)
        if song is not None and song.playing_status == LoggedSong.STATUS_PLAYING:
            self.diaglog.debug("Unknown - setting playing_song to %s", song)
            self.playing_song = song
            self.playback_start_time = datetime.now()
        else:
            self.diaglog.debug("Unknown - setting playing_song to none")
            self.playing_song = None

    def shutting_down(self):
        if self.playing_song is None:
            self.diaglog.debug("shutdown - Playing song is none.")
            return
        if self.playcount_enabled:
            self.diaglog.debug("shutdown - Trying to picke playing song - not none")
            if self.playing_song.playing_status == LoggedSong.STATUS_PLAYING:
                # We set it to pause. Otherwise on next start it is possible for software to detect as restart playing
   the same song. And do additional playcount. self.playing_song.playing_status = LoggedSong.STATUS_PAUSE
                self.playing_song.playing_seconds += (datetime.now() - self.playback_start_time).seconds
        pickle_where = os.path.join(os.path.dirname(exec_beets("config -p")), "lastSong.bin")
        with open(pickle_where, "wb") as fp:
            pickle.dump(self.playing_song, fp, -1)
            self.diaglog.debug("shutdown - Pickle OK. Song is %s", self.playing_song)

 */
