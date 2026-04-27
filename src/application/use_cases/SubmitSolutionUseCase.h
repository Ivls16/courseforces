#pragma once

#include <optional>
#include <string>

#include "domain/Id.h"
#include "domain/entities/Submission.h"
#include "domain/repositories/IContestRepository.h"
#include "domain/repositories/IProblemRepository.h"
#include "domain/repositories/ISubmissionRepository.h"
#include "domain/repositories/IUserRepository.h"

namespace cf::application {

class SubmitSolutionUseCase {
public:
    struct Input {
        domain::Id user_id;
        domain::Id problem_id;
        std::optional<domain::Id> contest_id;
        std::string code;
        std::string language;
    };

    SubmitSolutionUseCase(domain::IUserRepository& users,
                          domain::IProblemRepository& problems,
                          domain::IContestRepository& contests,
                          domain::ISubmissionRepository& submissions);

    domain::Submission execute(const Input& input);

private:
    domain::IUserRepository& users_;
    domain::IProblemRepository& problems_;
    domain::IContestRepository& contests_;
    domain::ISubmissionRepository& submissions_;
};

}
