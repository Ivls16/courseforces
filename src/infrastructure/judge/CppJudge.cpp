#include "infrastructure/judge/CppJudge.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace cf::infrastructure {

CppJudge::CppJudge(std::filesystem::path workdir) : workdir_(std::move(workdir)) {
    std::filesystem::create_directories(workdir_);
}

bool CppJudge::compile(const std::filesystem::path& src,
                       const std::filesystem::path& bin,
                       std::string& error_out) const {
    std::filesystem::path err_file = std::filesystem::path(src).replace_extension(".err");
    std::string cmd =
        "g++ -O2 -std=c++17 -o " + bin.string() +
        " " + src.string() +
        " 2> " + err_file.string();

    int ret = std::system(cmd.c_str());

    std::ifstream err_stream(err_file);
    error_out.assign(std::istreambuf_iterator<char>(err_stream),
                     std::istreambuf_iterator<char>());
    std::filesystem::remove(err_file);

    return (ret == 0);
}

domain::TestCaseResult CppJudge::run_one(const std::filesystem::path& bin,
                                          const domain::TestCase& tc,
                                          int time_limit_ms,
                                          int memory_limit_mb) const {
    namespace fs = std::filesystem;

    fs::path in_file  = workdir_ / "tc.in";
    fs::path out_file = workdir_ / "tc.out";

    {
        std::ofstream f(in_file);
        f << tc.input();
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("fork() failed");
    }

    if (pid == 0) {
        // ---- child ----
        // redirect stdin from in_file
        FILE* fin = std::fopen(in_file.c_str(), "r");
        dup2(fileno(fin), STDIN_FILENO);
        // redirect stdout to out_file
        FILE* fout = std::fopen(out_file.c_str(), "w");
        dup2(fileno(fout), STDOUT_FILENO);
        // silence stderr
        FILE* fnull = std::fopen("/dev/null", "w");
        dup2(fileno(fnull), STDERR_FILENO);

        // CPU time limit (seconds, rounded up)
        rlimit cpu{};
        cpu.rlim_cur = static_cast<rlim_t>((time_limit_ms + 999) / 1000);
        cpu.rlim_max = cpu.rlim_cur + 1;
        setrlimit(RLIMIT_CPU, &cpu);

        // Virtual memory limit
        rlimit mem{};
        mem.rlim_cur = static_cast<rlim_t>(memory_limit_mb) * 1024 * 1024;
        mem.rlim_max = mem.rlim_cur;
        setrlimit(RLIMIT_AS, &mem);

        execl(bin.c_str(), bin.c_str(), nullptr);
        _exit(127);
    }

    // ---- parent ----
    int status = 0;
    struct rusage usage{};
    wait4(pid, &status, 0, &usage);

    long time_used_ms = static_cast<long>(usage.ru_utime.tv_sec) * 1000
                      + usage.ru_utime.tv_usec / 1000;

    long memory_used_kb = usage.ru_maxrss;  // Linux: kilobytes

    domain::Verdict verdict = domain::Verdict::Accepted;

    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        verdict = (sig == SIGXCPU) ? domain::Verdict::TimeLimitExceeded
                                   : domain::Verdict::RuntimeError;
    } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        verdict = domain::Verdict::RuntimeError;
    } else {
        // compare output
        std::ifstream out_stream(out_file);
        std::string actual_output(std::istreambuf_iterator<char>(out_stream),
                                  std::istreambuf_iterator<char>{});

        auto trim = [](std::string s) -> std::string {
            const auto ws = [](char c){ return c == '\n' || c == '\r' || c == ' ' || c == '\t'; };
            auto first = std::find_if_not(s.begin(), s.end(), ws);
            auto last  = std::find_if_not(s.rbegin(), s.rend(), ws).base();
            return (first < last) ? std::string(first, last) : std::string{};
        };
        if (trim(actual_output) != trim(tc.expected_output())) {
            verdict = domain::Verdict::WrongAnswer;
        }
    }

    std::filesystem::remove(in_file);
    std::filesystem::remove(out_file);

    return {verdict,
            static_cast<int>(time_used_ms),
            static_cast<int>(memory_used_kb),
            ""};
}

domain::JudgeReport CppJudge::judge(const domain::Submission& submission,
                                     const domain::Problem& problem) {
    namespace fs = std::filesystem;

    std::string name = "sub_" + std::to_string(submission.id());
    fs::path src = workdir_ / (name + ".cpp");
    fs::path bin = workdir_ / name;

    {
        std::ofstream f(src);
        f << submission.code();
    }

    domain::JudgeReport report;

    std::string compile_error;
    if (!compile(src, bin, compile_error)) {
        fs::remove(src);
        report.overall = domain::Verdict::CompilationError;
        report.compile_error = compile_error;
        return report;
    }
    fs::remove(src);

    report.overall = domain::Verdict::Accepted;

    for (const auto& tc : problem.test_cases()) {
        auto result = run_one(bin, tc, problem.time_limit_ms(), problem.memory_limit_mb());
        report.per_case.push_back(result);
        // first non-AC verdict wins
        if (result.verdict != domain::Verdict::Accepted &&
            report.overall == domain::Verdict::Accepted) {
            report.overall = result.verdict;
        }
    }

    // no test cases → treat as AC (judge decides)
    fs::remove(bin);
    return report;
}

}
