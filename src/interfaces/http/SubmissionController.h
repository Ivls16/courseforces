#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>

#include "application/use_cases/JudgeSubmissionUseCase.h"
#include "application/use_cases/SubmitSolutionUseCase.h"
#include "domain/repositories/ISubmissionRepository.h"

namespace cf::interfaces::http {

class SubmissionController {
public:
    SubmissionController(application::SubmitSolutionUseCase& submit_uc,
                         application::JudgeSubmissionUseCase& judge_uc,
                         domain::ISubmissionRepository& submissions);

    void register_routes(crow::SimpleApp& app);

private:
    application::SubmitSolutionUseCase& submit_uc_;
    application::JudgeSubmissionUseCase& judge_uc_;
    domain::ISubmissionRepository& submissions_;

    crow::response handle_submit(const crow::request& req, domain::Id contest_id);
    crow::response handle_get(domain::Id id);
    crow::response handle_by_user(domain::Id user_id);
};

}
