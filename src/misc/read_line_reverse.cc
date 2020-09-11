#include <algorithm>
#include <cassert>
#include <iostream>
#include <src/misc/custom_include_spdlog.hpp>
#include <src/misc/read_line_reverse.hpp>

bool ReadLineReverse::read_some_buffer() {
  file_read_location_ -= LINE_SIZE_STEP;
  SPDLOG_TRACE("buffering ...");

  const auto should_read =
      static_cast<size_t>(file_read_location_ < 0 ? LINE_SIZE_STEP + file_read_location_ : LINE_SIZE_STEP);
  if (file_read_location_ < 0) file_read_location_ = 0;
  if (should_read == 0) {
    SPDLOG_TRACE(" ! buffering TO END .. .ret false ...");
    return false;
  }
  istream_->seekg(file_read_location_, std::ios::beg);
  buffer_.insert(buffer_.begin(), should_read, 0);
  istream_->read(buffer_.data(), should_read);
  return true;
}

void ReadLineReverse::find_rightmost_newline() {
  const auto end_newline_where = std::rbegin(buffer_) + 3;
  const auto end_of_file_nl = std::find(std::rbegin(buffer_), end_newline_where, '\n');
  if (end_of_file_nl == end_newline_where) {
    // not found ... insert newline to buffer and retry.
    buffer_.push_back('\n');
    find_rightmost_newline();
    return;
  }
  reverse_idx_start_of_line_that_is_to_be_read_next_ = std::distance(std::rbegin(buffer_), end_of_file_nl) - 1;
  assert(idx_unreverse(reverse_idx_start_of_line_that_is_to_be_read_next_) == 0 ||
         buffer_[idx_unreverse(reverse_idx_start_of_line_that_is_to_be_read_next_ + 1)] == '\n');
}

void ReadLineReverse::advance_indices() {
  const auto stored_reverse_idx_start_of_line_that_is_to_be_read_next =
      reverse_idx_start_of_line_that_is_to_be_read_next_;
  if (idx_unreverse(stored_reverse_idx_start_of_line_that_is_to_be_read_next) == 0) {
    throw std::runtime_error("No more data!");
  }
  while (true) {
    // find next newline
    auto finding_newline_first_position_reverse =
        std::rbegin(buffer_) + static_cast<size_t>(stored_reverse_idx_start_of_line_that_is_to_be_read_next + 2);
    assert(*(finding_newline_first_position_reverse - 1) == '\n' &&
           "This one must be one to the left of \\n - location where we stop reading. -1 pushes it one to right ... +1 "
           "would push it one to left. ");
    auto it_found_left_newline = std::find(finding_newline_first_position_reverse, std::rend(buffer_), '\n');
    if (it_found_left_newline == std::rend(buffer_)) {
      const auto moved = read_some_buffer();
      if (!moved) {
        reverse_idx_start_of_line_that_is_to_be_read_next_ = idx_unreverse(buffer_, 0);
        break;
      }
    } else {
      assert(*it_found_left_newline == '\n');
      SPDLOG_TRACE("CalcIdx {}", idx_unreverse(it_found_left_newline - std::rbegin(buffer_)));
      assert(idx_unreverse(it_found_left_newline - std::rbegin(buffer_)) >= 0);
      assert(buffer_[idx_unreverse(it_found_left_newline - std::rbegin(buffer_))] == '\n');
      reverse_idx_start_of_line_that_is_to_be_read_next_ = it_found_left_newline - std::rbegin(buffer_) - 1;
      assert(buffer_[idx_unreverse(reverse_idx_start_of_line_that_is_to_be_read_next_) - 1] == '\n');
      break;
    }
  }
  reverse_idx_newline_to_the_right_of_to_read_ = stored_reverse_idx_start_of_line_that_is_to_be_read_next + 1;
  assert(buffer_[idx_unreverse(reverse_idx_newline_to_the_right_of_to_read_)] == '\n');
}

ReadLineReverse::ReadLineReverse(std::unique_ptr<std::istream> istream) : istream_(std::move(istream)) {
  buffer_.reserve(LINE_SIZE_STEP + 1);
  istream_->seekg(0, std::ios_base::end);
  // file_size_ = ifstream_.tellg();
  file_read_location_ = static_cast<int64_t>(istream_->tellg());

  read_some_buffer();

  // Newline at end?
  find_rightmost_newline();
}

std::string ReadLineReverse::next() {
  advance_indices();

  auto idx_r = idx_unreverse(buffer_, static_cast<size_t>(reverse_idx_newline_to_the_right_of_to_read_));
  if (*(buffer_.begin() + idx_r - 1) == '\r') {
    --idx_r;
  }
  const auto idx_left = idx_unreverse(buffer_, static_cast<size_t>(reverse_idx_start_of_line_that_is_to_be_read_next_));
  assert(idx_r >= idx_left);

  return std::string(buffer_.begin() + idx_left, buffer_.begin() + idx_r);
}
