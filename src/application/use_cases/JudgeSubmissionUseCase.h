#pragma once

#include "domain/Id.h"
#include "domain/entities/Submission.h"
#include "domain/repositories/IProblemRepository.h"
#include "domain/repositories/ISubmissionRepository.h"
#include "domain/services/IJudge.h"

namespace cf::application {

class JudgeSubmissionUseCase {
public:
    JudgeSubmissionUseCase(domain::ISubmissionRepository& submissions,
                           domain::IProblemRepository& problems,
                           domain::IJudge& judge);

    domain::Submission execute(domain::Id submission_id);

private:
    domain::ISubmissionRepository& submissions_;
    domain::IProblemRepository& problems_;
    domain::IJudge& judge_;
};

}
