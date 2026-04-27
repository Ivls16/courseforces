#include "application/use_cases/JudgeSubmissionUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

JudgeSubmissionUseCase::JudgeSubmissionUseCase(domain::ISubmissionRepository& submissions,
                                               domain::IProblemRepository& problems,
                                               domain::IJudge& judge)
    : submissions_(submissions), problems_(problems), judge_(judge) {}

domain::Submission JudgeSubmissionUseCase::execute(domain::Id submission_id) {
    auto submission = submissions_.find_by_id(submission_id);
    if (!submission.has_value()) {
        throw domain::NotFoundException("submission not found");
    }
    auto problem = problems_.find_by_id(submission->problem_id());
    if (!problem.has_value()) {
        throw domain::NotFoundException("problem not found for submission");
    }

    auto report = judge_.judge(*submission, *problem);
    submission->set_verdict(report.overall);
    submissions_.update(*submission);
    return *submission;
}

}
