#pragma once
#include <vector>
#include <fstream>
#include <memory>


class ReadLineReverse {
  std::unique_ptr<std::istream> istream_{};

  constexpr static int LINE_SIZE_STEP = 250;
  std::vector<char> buffer_;

  int64_t reverse_idx_start_of_line_that_is_to_be_read_next_{};
  int64_t reverse_idx_newline_to_the_right_of_to_read_{};
  int64_t file_read_location_;

  bool read_some_buffer();

  template <typename t_container, typename number_type>
  static number_type idx_unreverse(const t_container& cont, number_type item) {
    const auto right = static_cast<number_type>(cont.size() - 1);
    return right - item;
  }

  template <typename number_type>
  number_type idx_unreverse(number_type item) {
    return idx_unreverse(buffer_, item);
  }

  void find_rightmost_newline();
  void advance_indices();
public:


  explicit ReadLineReverse(std::unique_ptr<std::istream> istream);

  std::string next();


  ReadLineReverse(const ReadLineReverse& other) = delete;

  ReadLineReverse(ReadLineReverse&& other) noexcept = default;
  ReadLineReverse& operator=(const ReadLineReverse& other) = delete;

  ReadLineReverse& operator=(ReadLineReverse&& other) noexcept {
    if (this == &other)
      return *this;
    istream_ = std::move(other.istream_);
    buffer_ = std::move(other.buffer_);
    reverse_idx_start_of_line_that_is_to_be_read_next_ = other.reverse_idx_start_of_line_that_is_to_be_read_next_;
    reverse_idx_newline_to_the_right_of_to_read_ = other.reverse_idx_newline_to_the_right_of_to_read_;
    file_read_location_ = other.file_read_location_;
    return *this;
  }
};
