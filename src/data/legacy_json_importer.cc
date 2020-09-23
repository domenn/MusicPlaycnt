#include "legacy_json_importer.hpp"

#include <fstream>
#include <src/misc/custom_include_spdlog.hpp>
#include <src/misc/utilities.hpp>
#include <src/model/song_list.hpp>

// std::string single_string()

template <typename json_t, typename variable_type = std::string>
void add_if_has(const char *const json_key,
                msw::model::Song *song,
                json_t json,
                void (msw::model::Song::*setter)(std::string &&)) {
  try {
    auto thingy = msw::helpers::Utilities::trim(json[json_key]);
    if (!thingy.empty()) {
      (song->*setter)(std::move(thingy));
    }
  } catch (std::exception &any) {
    SPDLOG_ERROR("NOT IMPLD ... TBD");
    throw any;
  }
}

void LegacyJsonImporter::iterate_and_import(const nlohmann::json &json) {
  msw::model::SongList new_song_list;
  for (auto &entry : json) {
    msw::model::Song song(msw::helpers::Utilities::trim(entry["album"]),
                          msw::helpers::Utilities::trim(entry["artist"]),
                          msw::helpers::Utilities::trim(entry["title"]),
                          msw::helpers::Utilities::shorten_path(msw::helpers::Utilities::trim(entry["path"])));

    add_if_has("genre", &song, entry, &msw::model::Song::set_genre);

    /*
        "album": "The Nexus (Deluxe Edition)",
        "artist": "Amaranthe",
        "genre": " ",
        "path": "C:\\Bacup_SSD_DiskWd\\syncMuzq\\All\\filtered\\Amaranthe - Discography (2011-2016) (Lossless)\\(01)
       [Amaranthe] Afterlife.flac", "playcnt": "9", "title": "Afterlife", "usertags": "dqawsome"
     */

    // for (auto &song_item : entry.get<nlohmann::json::object_t>()) {
    //  std::string current_json_value(song_item.second);
    //  if (msw::helpers::Utilities::is_empty_or_spaces(current_json_value)) {
    //    continue;
    //  }

    //  sqls_builder.add_song_property(song_item.first, song_item.second);
    //  // LOG_DEBUG << "Line: " << song_item.first << " ;;; " << song_item.second;
    //}
    // sqls_builder.build_and_update_db(db_shared_);
  }
}

LegacyJsonImporter::LegacyJsonImporter(std::string &&json_input_path)
    : LegacyJsonImporter(std::ifstream(json_input_path)) {}

LegacyJsonImporter::LegacyJsonImporter(std::istream &&input_stream) {
  nlohmann::json legacy_data_json;
  input_stream >> legacy_data_json;
  iterate_and_import(legacy_data_json);
}

LegacyJsonImporter::~LegacyJsonImporter() = default;
