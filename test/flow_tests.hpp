#pragma once

#include "util/data_for_tests.hpp"
#include "util/string_provider.hpp"
#include <src\musicstuff\do_things.cc>

TEST(Flow, on_pause_songs_differ) {
  msw::pg::handled_song->replace(SongWithMetadata(msw::model::Song("a", "b", "c", msw::p_mk("d")), ActionType::PAUSE));

  SongWithMetadata s_new(msw::model::Song("b", "c", "f", msw::p_mk("d")), ActionType::STOP);

  // ASSERT_EQ(msw::pg::handled_song->read()->get_song().artist(), VALUE);
  // const std::string& serval = msw::pg::data_for_tests->serialized_;
  // ASSERT_NE(serval.find(VALUE), std::string::npos)
  //    << "Test-content serialized string doesn't contain the expected word (" << VALUE << ")";

  msw::do_things::Handler handler(std::move(s_new));
  // this should trigger stop handler.
  handler.update_data_for_currently_stored_song();
}