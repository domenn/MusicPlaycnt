#include "tests_main.hpp"

#include <sago/platform_folders.h>

#include <src/data/accessor.hpp>
#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

#include "persistence_test.hpp"
#include "src/misc/consts.hpp"
#include "src/misc/utilities.hpp"

namespace msw::pg {
data::Accessor<msw::model::SongList>* song_list;
data::Accessor<msw::model::SongWithMetadata>* handled_song;
}

using string = std::string;

//void* data_;
//std::function<void(void*, T*)> persist_function_;
//std::function<std::unique_ptr<T>(void*)> initial_load_function_;
//


template <typename T>
void persist_for_tests(void* data, T* serializable) {
  auto* actual_data = static_cast<DataForTests*>(data);
  actual_data->serialized_ = serializable->serialize_to_str();
}

template <typename T>
std::unique_ptr<T> initial_load_for_tests(void* data) {
  return std::make_unique<T>();
}

int main(int argc, char** argv) {
  spdl::spdlog_setup(spdl::SpdlogConfig::build()
                     .default_logger_name("Music2_test")
                     .file_name("NewMusicTrackerCounter.log")
                     .log_to_file(false)
                     .level(spdlog::level::debug)
                     .pattern(spdl::SpdlogConfig::PATTERN_ALL_DATA));

  // msw::data::Accessor<msw::model::SongList> inst_song_list;
  DataForTests data_for_song_with_metadata_;
  msw::data::Accessor<msw::model::SongWithMetadata> inst_handled_song(&data_for_song_with_metadata_,
                                                                      &persist_for_tests<msw::model::SongWithMetadata>,
                                                                      &initial_load_for_tests<msw::model::
                                                                        SongWithMetadata>);
  // msw::pg::song_list = &inst_song_list;
  msw::pg::handled_song = &inst_handled_song;

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

std::string msw::helpers::Utilities::app_folder() {
  return sago::getConfigHome() + '/' + consts::TESTS_DIR;
}

std::string msw::StringProvider::get_str(std::string&& prefix) {
  string right_part = std::to_string(seq++);
  return std::move(prefix) + "_testStr__" + right_part + right_part + right_part + ".one";
}

std::tuple<std::string, std::string, std::string, std::string> msw::StringProvider::four_strings(std::string&& prefix) {
  return std::make_tuple(get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::string(prefix)),
                         get_str(std::move(prefix)));
}


std::string msw::musicstuffs::FooNpLogParser::extract_last_line() const {
  return (reinterpret_cast<const MockFooNpLogParser*>(this))->line_to_parse_;
}
