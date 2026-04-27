#include "interfaces/http/SubmissionController.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::interfaces::http {

namespace {

nlohmann::json submission_to_json(const domain::Submission& s) {
    nlohmann::json j = {
        {"id",         s.id()},
        {"user_id",    s.user_id()},
        {"problem_id", s.problem_id()},
        {"language",   s.language()},
        {"code",       s.code()},
        {"verdict",    std::string(domain::to_string(s.verdict()))},
        {"is_judged",  s.is_judged()}
    };
    if (s.contest_id().has_value()) j["contest_id"] = *s.contest_id();
    return j;
}

crow::response error_resp(int code, const std::string& msg) {
    crow::response r(code, nlohmann::json{{"error", msg}}.dump());
    r.set_header("Content-Type", "application/json");
    return r;
}

crow::response json_resp(int code, const nlohmann::json& body) {
    crow::response r(code, body.dump());
    r.set_header("Content-Type", "application/json");
    return r;
}

}

SubmissionController::SubmissionController(application::SubmitSolutionUseCase& submit_uc,
                                           application::JudgeSubmissionUseCase& judge_uc,
                                           domain::ISubmissionRepository& submissions)
    : submit_uc_(submit_uc), judge_uc_(judge_uc), submissions_(submissions) {}

void SubmissionController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/contests/<int>/submit").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req, int contest_id) {
            return handle_submit(req, contest_id);
        });

    CROW_ROUTE(app, "/api/submissions/<int>").methods(crow::HTTPMethod::GET)(
        [this](int id) { return handle_get(id); });

    CROW_ROUTE(app, "/api/users/<int>/submissions").methods(crow::HTTPMethod::GET)(
        [this](int user_id) { return handle_by_user(user_id); });
}

crow::response SubmissionController::handle_submit(const crow::request& req,
                                                    domain::Id contest_id) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto sub = submit_uc_.execute({
            body.at("user_id").get<domain::Id>(),
            body.at("problem_id").get<domain::Id>(),
            contest_id,
            body.at("code").get<std::string>(),
            body.value("language", "cpp")
        });
        // synchronous judging in the same request
        auto judged = judge_uc_.execute(sub.id());
        return json_resp(201, submission_to_json(judged));
    } catch (const domain::NotFoundException& e) {
        return error_resp(404, e.what());
    } catch (const domain::DomainException& e) {
        return error_resp(400, e.what());
    } catch (const nlohmann::json::exception&) {
        return error_resp(400, "malformed JSON");
    }
}

crow::response SubmissionController::handle_get(domain::Id id) {
    auto s = submissions_.find_by_id(id);
    if (!s.has_value()) return error_resp(404, "submission not found");
    return json_resp(200, submission_to_json(*s));
}

crow::response SubmissionController::handle_by_user(domain::Id user_id) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& s : submissions_.by_user(user_id))
        arr.push_back(submission_to_json(s));
    return json_resp(200, arr);
}

}
