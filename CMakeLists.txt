cmake_minimum_required(VERSION 3.16)
project(courseforces CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# === Dependencies ===

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG        3.3.2
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        v1.2.0
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)

set(CROW_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT  OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON  CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json SQLiteCpp asio googletest)

set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "" FORCE)

FetchContent_MakeAvailable(Crow)

# === Core library (domain + application + infrastructure) ===

add_library(courseforces_core STATIC
    # Domain
    src/domain/entities/Verdict.cpp
    src/domain/entities/Role.cpp
    src/domain/entities/User.cpp
    src/domain/entities/TestCase.cpp
    src/domain/entities/Problem.cpp
    src/domain/entities/Contest.cpp
    src/domain/entities/Submission.cpp
    # Application
    src/application/use_cases/RegisterUserUseCase.cpp
    src/application/use_cases/LoginUserUseCase.cpp
    src/application/use_cases/CreateProblemUseCase.cpp
    src/application/use_cases/SubmitSolutionUseCase.cpp
    src/application/use_cases/JudgeSubmissionUseCase.cpp
    src/application/use_cases/CreateContestUseCase.cpp
    # Infrastructure
    src/infrastructure/db/SqlSchema.cpp
    src/infrastructure/db/SqliteUserRepository.cpp
    src/infrastructure/db/SqliteProblemRepository.cpp
    src/infrastructure/db/SqliteContestRepository.cpp
    src/infrastructure/db/SqliteSubmissionRepository.cpp
    src/infrastructure/auth/Sha256PasswordHasher.cpp
    src/infrastructure/judge/CppJudge.cpp
)
target_include_directories(courseforces_core PUBLIC src third_party)
target_link_libraries(courseforces_core PUBLIC SQLiteCpp nlohmann_json::nlohmann_json)

# === HTTP interface adapters (depend on Crow) ===

add_library(courseforces_http STATIC
    src/interfaces/http/UserController.cpp
    src/interfaces/http/ProblemController.cpp
    src/interfaces/http/SubmissionController.cpp
    src/interfaces/http/ContestController.cpp
)
target_include_directories(courseforces_http PUBLIC src)
target_link_libraries(courseforces_http PUBLIC courseforces_core Crow::Crow)

# === Server ===

add_executable(courseforces_server src/main.cpp)
target_link_libraries(courseforces_server PRIVATE
    courseforces_http
    Crow::Crow
)

# === CLI client ===

add_executable(courseforces_cli cli/main.cpp)
target_include_directories(courseforces_cli PRIVATE third_party)
target_link_libraries(courseforces_cli PRIVATE
    nlohmann_json::nlohmann_json
)

# === Tests ===

enable_testing()

add_executable(courseforces_tests
    # Domain
    tests/domain/VerdictTest.cpp
    tests/domain/RoleTest.cpp
    tests/domain/UserTest.cpp
    tests/domain/TestCaseTest.cpp
    tests/domain/ProblemTest.cpp
    tests/domain/ContestTest.cpp
    tests/domain/SubmissionTest.cpp
    # Application
    tests/application/RegisterUserUseCaseTest.cpp
    tests/application/LoginUserUseCaseTest.cpp
    tests/application/CreateProblemUseCaseTest.cpp
    tests/application/SubmitSolutionUseCaseTest.cpp
    tests/application/JudgeSubmissionUseCaseTest.cpp
    tests/application/CreateContestUseCaseTest.cpp
    # Infrastructure
    tests/infrastructure/SqliteUserRepositoryTest.cpp
    tests/infrastructure/SqliteProblemRepositoryTest.cpp
    tests/infrastructure/SqliteSubmissionRepositoryTest.cpp
    tests/infrastructure/Sha256PasswordHasherTest.cpp
    tests/infrastructure/CppJudgeTest.cpp
)
target_include_directories(courseforces_tests PRIVATE src tests third_party)
target_link_libraries(courseforces_tests PRIVATE
    courseforces_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(courseforces_tests)
