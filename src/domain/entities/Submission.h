#pragma once

#include <optional>
#include <string>

#include "domain/Id.h"
#include "domain/entities/Verdict.h"

namespace cf::domain {

class Submission {
public:
    Submission(Id id,
               Id user_id,
               Id problem_id,
               std::optional<Id> contest_id,
               std::string code,
               std::string language);

    Id id() const noexcept { return id_; }
    Id user_id() const noexcept { return user_id_; }
    Id problem_id() const noexcept { return problem_id_; }
    std::optional<Id> contest_id() const noexcept { return contest_id_; }
    const std::string& code() const noexcept { return code_; }
    const std::string& language() const noexcept { return language_; }
    Verdict verdict() const noexcept { return verdict_; }

    void assign_id(Id new_id);
    void set_verdict(Verdict v) noexcept;

    bool is_judged() const noexcept;

private:
    Id id_;
    Id user_id_;
    Id problem_id_;
    std::optional<Id> contest_id_;
    std::string code_;
    std::string language_;
    Verdict verdict_ = Verdict::Pending;
};

}
