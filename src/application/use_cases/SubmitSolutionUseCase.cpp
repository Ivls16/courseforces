#include "application/use_cases/SubmitSolutionUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

SubmitSolutionUseCase::SubmitSolutionUseCase(domain::IUserRepository& users,
                                             domain::IProblemRepository& problems,
                                             domain::IContestRepository& contests,
                                             domain::ISubmissionRepository& submissions)
    : users_(users), problems_(problems), contests_(contests), submissions_(submissions) {}

domain::Submission SubmitSolutionUseCase::execute(const Input& input) {
    if (!users_.find_by_id(input.user_id).has_value()) {
        throw domain::NotFoundException("user not found");
    }
    if (!problems_.find_by_id(input.problem_id).has_value()) {
        throw domain::NotFoundException("problem not found");
    }
    if (input.contest_id.has_value() &&
        !contests_.find_by_id(*input.contest_id).has_value()) {
        throw domain::NotFoundException("contest not found");
    }

    domain::Submission submission(domain::kInvalidId,
                                  input.user_id,
                                  input.problem_id,
                                  input.contest_id,
                                  input.code,
                                  input.language);

    domain::Id new_id = submissions_.create(submission);
    submission.assign_id(new_id);
    return submission;
}

}
