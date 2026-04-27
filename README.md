# CourseForces

Платформа для проведения соревнований по программированию.  
Курсовой проект по дисциплине **ООП**, 4 семестр.

---

## Архитектура

Проект реализован на C++17 по принципу **Clean Architecture** — зависимости направлены строго внутрь.

```
┌─────────────────────────────────────────────┐
│  Infrastructure  (Crow, SQLite, fork/exec)  │
├─────────────────────────────────────────────┤
│  Interface Adapters  (HTTP-контроллеры)     │
├─────────────────────────────────────────────┤
│  Application  (Use Cases)                  │
├─────────────────────────────────────────────┤
│  Domain  (Entities, Repository interfaces) │
└─────────────────────────────────────────────┘
```

Применённые паттерны ООП: **Repository**, **Strategy** (IJudge), **Factory** (JudgeFactory), **Dependency Injection** через конструктор, **Template Method**.

---

## Стек

| Компонент           | Библиотека                                      |
|---------------------|-------------------------------------------------|
| HTTP-сервер         | [Crow 1.2](https://github.com/CrowCpp/Crow)     |
| База данных         | SQLite 3 через [SQLiteCpp 3.3](https://github.com/SRombauts/SQLiteCpp) |
| JSON                | [nlohmann/json 3.11](https://github.com/nlohmann/json) |
| Хеш паролей        | SHA-256 через [PicoSHA2](https://github.com/okdshin/PicoSHA2) |
| HTTP-клиент (CLI)   | [cpp-httplib 0.18](https://github.com/yhirose/cpp-httplib) |
| Тесты               | [GoogleTest 1.15](https://github.com/google/googletest) |
| Сборка              | CMake ≥ 3.16                                    |

---

## Требования

- Linux (Ubuntu 22.04+)
- `g++-13` и `gcc-13`
- `cmake` ≥ 3.16
- Доступ в интернет при первой сборке (FetchContent скачивает зависимости)

Установка компилятора на Ubuntu 22.04:
```bash
sudo apt install g++-13 gcc-13
```

---

## Сборка

```bash
git clone <repo-url>
cd courseforces

cmake -S . -B build \
      -DCMAKE_CXX_COMPILER=g++-13 \
      -DCMAKE_C_COMPILER=gcc-13

cmake --build build -j$(nproc)
```

После сборки в папке `build/` появятся:
- `courseforces_server` — REST API-сервер
- `courseforces_cli`    — консольный клиент
- `courseforces_tests`  — тесты

---

## Запуск сервера

```bash
./build/courseforces_server
```

Сервер стартует на порту **18080**. База данных `courseforces.db` создаётся рядом с исполняемым файлом при первом запуске.

Сменить порт можно, изменив значение в `src/main.cpp` (`.port(18080)`).

---

## REST API

### Аутентификация

| Метод | URL                   | Тело запроса                                    | Описание              |
|-------|-----------------------|-------------------------------------------------|-----------------------|
| POST  | `/api/auth/register`  | `{"username","password","role"}`                | Регистрация           |
| POST  | `/api/auth/login`     | `{"username","password"}`                       | Вход                  |

Доступные роли: `PARTICIPANT`, `JUDGE`, `ADMIN`.

### Задачи

| Метод | URL                | Тело запроса                                               | Права       |
|-------|--------------------|------------------------------------------------------------|-------------|
| GET   | `/api/problems`    | —                                                          | все         |
| GET   | `/api/problems/:id`| —                                                          | все         |
| POST  | `/api/problems`    | `{"actor_id","title","statement","time_limit_ms","memory_limit_mb","test_cases":[{"input","expected_output"}]}` | JUDGE/ADMIN |

### Контесты

| Метод | URL                | Тело запроса                                                  | Права |
|-------|--------------------|---------------------------------------------------------------|-------|
| GET   | `/api/contests`    | —                                                             | все   |
| GET   | `/api/contests/:id`| —                                                             | все   |
| POST  | `/api/contests`    | `{"actor_id","title","start_time","end_time","problem_ids":[]}` | ADMIN |

`start_time` и `end_time` — Unix-время (секунды).

### Сабмиты

| Метод | URL                              | Тело запроса                             | Описание                              |
|-------|----------------------------------|------------------------------------------|---------------------------------------|
| POST  | `/api/contests/:id/submit`       | `{"user_id","problem_id","code","language"}` | Отправить решение, сразу вернёт вердикт |
| GET   | `/api/submissions/:id`           | —                                        | Получить результат сабмита            |

Вердикты: `PENDING` / `AC` / `WA` / `TLE` / `MLE` / `CE` / `RE`.

### Пример полного flow

```bash
BASE=http://localhost:18080

# Регистрация
curl -s -X POST $BASE/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"secret123","role":"ADMIN"}'

curl -s -X POST $BASE/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"pass1234","role":"PARTICIPANT"}'

# Создать задачу (от admin, id=1)
curl -s -X POST $BASE/api/problems \
  -H "Content-Type: application/json" \
  -d '{
    "actor_id": 1,
    "title": "A+B",
    "statement": "Вывести сумму двух чисел.",
    "time_limit_ms": 2000,
    "memory_limit_mb": 256,
    "test_cases": [
      {"input": "1 2", "expected_output": "3"},
      {"input": "10 20", "expected_output": "30"}
    ]
  }'

# Создать контест (от admin, id=1)
curl -s -X POST $BASE/api/contests \
  -H "Content-Type: application/json" \
  -d '{"actor_id":1,"title":"Round #1","start_time":0,"end_time":9999999999,"problem_ids":[1]}'

# Отправить решение (от alice, id=2)
curl -s -X POST $BASE/api/contests/1/submit \
  -H "Content-Type: application/json" \
  -d '{
    "user_id": 2,
    "problem_id": 1,
    "code": "#include<iostream>\nint main(){int a,b;std::cin>>a>>b;std::cout<<a+b;}",
    "language": "cpp"
  }'
```

---

## CLI-клиент

```bash
# Регистрация и вход
./build/courseforces_cli register admin   secret123 ADMIN
./build/courseforces_cli register alice   pass1234  PARTICIPANT
./build/courseforces_cli login    alice   pass1234

# Просмотр задач и контестов
./build/courseforces_cli problems
./build/courseforces_cli problem  1
./build/courseforces_cli contests
./build/courseforces_cli contest  1

# Отправка решения из файла
./build/courseforces_cli submit 1 2 1 solution.cpp

# Проверка вердикта
./build/courseforces_cli verdict 1
```

Переменные окружения:
```bash
CF_HOST=192.168.1.10   # хост сервера (по умолчанию localhost)
CF_PORT=9090           # порт сервера (по умолчанию 18080)
```

---

## Веб-интерфейс

Простой веб-фронтенд на **Python Flask + Jinja2** с [Pico CSS](https://picocss.com/).  
Фронтенд вызывает REST API сервера и отображает страницы через шаблоны.

### Установка зависимостей

```bash
pip install -r web/requirements.txt
```

### Запуск

Сначала запустите C++ сервер (порт 18080), затем веб-сервер:

```bash
./build/courseforces_server &
python3 web/app.py
```

Откройте браузер: **http://localhost:5000**

### Переменные окружения для фронтенда

```bash
CF_API=http://localhost:18080   # адрес C++ API (по умолчанию)
CF_WEB_PORT=5000                # порт веб-сервера (по умолчанию)
CF_SECRET=my-secret             # ключ для подписи сессии
```

### Страницы

| URL | Описание |
|-----|----------|
| `/contests` | Список контестов |
| `/contests/<id>` | Задачи контеста |
| `/contests/<id>/problems/<id>` | Условие + форма отправки решения |
| `/submissions/<id>` | Результат проверки |
| `/problems` | Список всех задач |
| `/problems/new` | Создать задачу (JUDGE / ADMIN) |
| `/contests/new` | Создать контест (ADMIN) |
| `/login`, `/register` | Аутентификация |

---

## Тесты

```bash
cd build
ctest --output-on-failure
```

79 тестов, покрывающих все слои:
- **Domain** — сущности и их валидация (33 теста)
- **Application** — use cases с fake-репозиториями (24 теста)
- **Infrastructure** — SQLite-репозитории, хешер паролей, CppJudge (22 теста)

---

## Структура проекта

```
courseforces/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── domain/
│   │   ├── entities/        User, Problem, Contest, Submission, Verdict, Role
│   │   ├── repositories/    IUserRepository, IProblemRepository, ...
│   │   ├── services/        IPasswordHasher, IJudge
│   │   └── exceptions/      DomainException, NotFoundException, ...
│   ├── application/
│   │   └── use_cases/       RegisterUser, Login, CreateProblem, CreateContest,
│   │                        SubmitSolution, JudgeSubmission
│   ├── infrastructure/
│   │   ├── db/              SqlSchema, SqliteUserRepository, ...
│   │   ├── auth/            Sha256PasswordHasher
│   │   └── judge/           CppJudge
│   └── interfaces/
│       └── http/            UserController, ProblemController,
│                            ContestController, SubmissionController
├── cli/
│   └── main.cpp             CLI-клиент
├── web/
│   ├── app.py               Flask-приложение (фронтенд)
│   ├── requirements.txt     flask, requests
│   └── templates/           Jinja2-шаблоны (base, login, register,
│                            contests, contest, problem, submission, ...)
├── tests/
│   ├── domain/
│   ├── application/
│   ├── infrastructure/
│   └── fakes/               InMemory*Repository, FakePasswordHasher, FakeJudge
└── third_party/
    ├── picosha2.h
    └── httplib.h
```
