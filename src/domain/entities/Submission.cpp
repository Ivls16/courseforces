#include "domain/entities/Submission.h"

#include <stdexcept>
#include <utility>

namespace cf::domain {

Submission::Submission(Id id,
                       Id user_id,
                       Id problem_id,
                       std::optional<Id> contest_id,
                       std::string code,
                       std::string language)
    : id_(id),
      user_id_(user_id),
      problem_id_(problem_id),
      contest_id_(contest_id),
      code_(std::move(code)),
      language_(std::move(language)) {
    if (user_id_ == kInvalidId) {
        throw std::invalid_argument("Submission: user_id must be valid");
    }
    if (problem_id_ == kInvalidId) {
        throw std::invalid_argument("Submission: problem_id must be valid");
    }
    if (code_.empty()) {
        throw std::invalid_argument("Submission: code must not be empty");
    }
    if (language_.empty()) {
        throw std::invalid_argument("Submission: language must not be empty");
    }
}

void Submission::assign_id(Id new_id) {
    if (id_ != kInvalidId) {
        throw std::logic_error("Submission: id is already assigned");
    }
    id_ = new_id;
}

void Submission::set_verdict(Verdict v) noexcept {
    verdict_ = v;
}

bool Submission::is_judged() const noexcept {
    return is_terminal(verdict_);
}

}
