#include "legacy_json_importer.hpp"

#include <fstream>
#include <src/data/accessor.hpp>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/custom_include_spdlog.hpp>
#include <src/misc/utilities.hpp>
#include <src/model/song_list.hpp>

// std::string single_string()

template <typename json_t>
void add_if_has(const char *const json_key,
                msw::model::Song *song,
                json_t json,
                void (msw::model::Song::*setter)(std::string &&)) {
  try {
    auto thingy = msw::helpers::Utilities::u8trim(json[json_key]);
    if (!thingy.empty()) {
      (song->*setter)(std::move(thingy));
    }
  } catch (nlohmann::detail::type_error &t_err) {
    const auto testing = std::string(t_err.what());
    if (t_err.id == 302 && testing.find("null") != std::string::npos) {
      return;
    }
    SPDLOG_ERROR("NOT IMPLD ... TBD {} {}", typeid(t_err).name(), t_err.what());
    throw t_err;
  }
}

template <typename setter_type, typename json_t>
void apply_if(const char *const json_key,
              msw::model::Song *song,
              json_t json,
              std::function<void(setter_type &&, msw::model::Song *)> f_callback) {
  try {
    f_callback(static_cast<setter_type>(json[json_key]), song);
  } catch (nlohmann::detail::type_error &t_err) {
    const auto testing = std::string(t_err.what());
    if (t_err.id == 302 && testing.find("null") != std::string::npos) {
      return;
    }
    throw t_err;
  }
}

void LegacyJsonImporter::iterate_and_import(const nlohmann::json &json) {
  msw::model::SongList new_song_list;
  for (auto &entry : json) {
    msw::model::Song song(msw::helpers::Utilities::u8trim(entry["album"]),
                          msw::helpers::Utilities::u8trim(entry["artist"]),
                          msw::helpers::Utilities::u8trim(entry["title"]),
                          msw::helpers::Utilities::shorten_path(msw::helpers::Utilities::u8trim(entry["path"])));

    add_if_has("genre", &song, entry, &msw::model::Song::set_genre);
    apply_if<std::string>("playcnt", &song, entry, [](std::string playcnt, msw::model::Song *lambda_s) {
      lambda_s->set_playcount(std::atoi(playcnt.c_str()));
    });
    apply_if<std::string>("usertags", &song, entry, [](std::string &&tags, msw::model::Song *lambda_s) {
      msw::helpers::Utilities::u8trim(tags);
      size_t idx_ending = 0;
      while (idx_ending < tags.size()) {
        const auto previous_pipe_loc_plus_one = idx_ending;
        idx_ending = tags.find('|', idx_ending);
        if (idx_ending == std::string::npos) {
          idx_ending = tags.size();
        }
        SPDLOG_TRACE("IdxL:{} IdxR:{} tags.substr(...):{} tags.size():{}",
                     previous_pipe_loc_plus_one,
                     idx_ending,
                     tags.substr(previous_pipe_loc_plus_one, idx_ending - previous_pipe_loc_plus_one),
                     tags.size());
        lambda_s->add_tag(tags.substr(previous_pipe_loc_plus_one, idx_ending - previous_pipe_loc_plus_one));
        ++idx_ending;
      }
    });
    new_song_list.add(std::move(song));
  }
  msw::pg::song_list->replace(std::move(new_song_list));
}

LegacyJsonImporter::LegacyJsonImporter(std::string &&json_input_path)
    : LegacyJsonImporter(std::ifstream(json_input_path)) {}

LegacyJsonImporter::LegacyJsonImporter(std::istream &&input_stream) {
  nlohmann::json legacy_data_json;
  input_stream >> legacy_data_json;
  iterate_and_import(legacy_data_json);
}

LegacyJsonImporter::~LegacyJsonImporter() = default;
