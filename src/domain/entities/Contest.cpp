#include "domain/entities/Contest.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace cf::domain {

Contest::Contest(Id id,
                 std::string title,
                 std::int64_t start_time_unix,
                 std::int64_t end_time_unix,
                 std::vector<Id> problem_ids)
    : id_(id),
      title_(std::move(title)),
      start_time_(start_time_unix),
      end_time_(end_time_unix),
      problem_ids_(std::move(problem_ids)) {
    if (title_.empty()) {
        throw std::invalid_argument("Contest: title must not be empty");
    }
    if (end_time_ <= start_time_) {
        throw std::invalid_argument("Contest: end_time must be after start_time");
    }
}

void Contest::assign_id(Id new_id) {
    if (id_ != kInvalidId) {
        throw std::logic_error("Contest: id is already assigned");
    }
    id_ = new_id;
}

void Contest::add_problem(Id problem_id) {
    if (problem_id == kInvalidId) {
        throw std::invalid_argument("Contest: cannot add invalid problem id");
    }
    if (std::find(problem_ids_.begin(), problem_ids_.end(), problem_id) != problem_ids_.end()) {
        return;
    }
    problem_ids_.push_back(problem_id);
}

bool Contest::has_started(std::int64_t now_unix) const noexcept {
    return now_unix >= start_time_;
}

bool Contest::has_ended(std::int64_t now_unix) const noexcept {
    return now_unix >= end_time_;
}

bool Contest::is_active(std::int64_t now_unix) const noexcept {
    return has_started(now_unix) && !has_ended(now_unix);
}

}
