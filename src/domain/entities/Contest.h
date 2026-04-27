#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "domain/Id.h"

namespace cf::domain {

class Contest {
public:
    Contest(Id id,
            std::string title,
            std::int64_t start_time_unix,
            std::int64_t end_time_unix,
            std::vector<Id> problem_ids = {});

    Id id() const noexcept { return id_; }
    const std::string& title() const noexcept { return title_; }
    std::int64_t start_time() const noexcept { return start_time_; }
    std::int64_t end_time() const noexcept { return end_time_; }
    const std::vector<Id>& problem_ids() const noexcept { return problem_ids_; }

    void assign_id(Id new_id);
    void add_problem(Id problem_id);

    bool has_started(std::int64_t now_unix) const noexcept;
    bool has_ended(std::int64_t now_unix) const noexcept;
    bool is_active(std::int64_t now_unix) const noexcept;

private:
    Id id_;
    std::string title_;
    std::int64_t start_time_;
    std::int64_t end_time_;
    std::vector<Id> problem_ids_;
};

}
