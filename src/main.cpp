#include <crow.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "application/use_cases/CreateContestUseCase.h"
#include "application/use_cases/CreateProblemUseCase.h"
#include "application/use_cases/JudgeSubmissionUseCase.h"
#include "application/use_cases/LoginUserUseCase.h"
#include "application/use_cases/RegisterUserUseCase.h"
#include "application/use_cases/SubmitSolutionUseCase.h"

#include "infrastructure/auth/Sha256PasswordHasher.h"
#include "infrastructure/db/SqlSchema.h"
#include "infrastructure/db/SqliteContestRepository.h"
#include "infrastructure/db/SqliteProblemRepository.h"
#include "infrastructure/db/SqliteSubmissionRepository.h"
#include "infrastructure/db/SqliteUserRepository.h"
#include "infrastructure/judge/CppJudge.h"

#include "interfaces/http/ContestController.h"
#include "interfaces/http/ProblemController.h"
#include "interfaces/http/SubmissionController.h"
#include "interfaces/http/UserController.h"

int main() {
    // --- Database ---
    SQLite::Database db("courseforces.db",
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    cf::infrastructure::SqlSchema::initialize(db);

    // --- Repositories ---
    cf::infrastructure::SqliteUserRepository       users(db);
    cf::infrastructure::SqliteProblemRepository    problems(db);
    cf::infrastructure::SqliteContestRepository    contests(db);
    cf::infrastructure::SqliteSubmissionRepository submissions(db);

    // --- Services ---
    cf::infrastructure::Sha256PasswordHasher hasher;
    cf::infrastructure::CppJudge             judge;

    // --- Use Cases ---
    cf::application::RegisterUserUseCase  register_uc(users, hasher);
    cf::application::LoginUserUseCase     login_uc(users, hasher);
    cf::application::CreateProblemUseCase create_problem_uc(users, problems);
    cf::application::CreateContestUseCase create_contest_uc(users, contests);
    cf::application::SubmitSolutionUseCase submit_uc(users, problems, contests, submissions);
    cf::application::JudgeSubmissionUseCase judge_uc(submissions, problems, judge);

    // --- HTTP server ---
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/health")([] {
        return crow::response(200,
            nlohmann::json{{"status", "ok"}}.dump());
    });

    cf::interfaces::http::UserController       user_ctrl(register_uc, login_uc);
    cf::interfaces::http::ProblemController    problem_ctrl(create_problem_uc, problems);
    cf::interfaces::http::ContestController    contest_ctrl(create_contest_uc, contests);
    cf::interfaces::http::SubmissionController submission_ctrl(submit_uc, judge_uc, submissions);

    user_ctrl.register_routes(app);
    problem_ctrl.register_routes(app);
    contest_ctrl.register_routes(app);
    submission_ctrl.register_routes(app);

    app.port(18080).multithreaded().run();
    return 0;
}
