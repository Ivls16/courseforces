import os
import requests
from flask import Flask, render_template, request, redirect, url_for, session, flash

app = Flask(__name__)
app.secret_key = os.environ.get("CF_SECRET", "dev-secret-key")


@app.template_filter("datetimeformat")
def datetimeformat(ts):
    from datetime import datetime
    try:
        return datetime.fromtimestamp(int(ts)).strftime("%d.%m.%Y %H:%M")
    except Exception:
        return str(ts)

API = os.environ.get("CF_API", "http://localhost:18080")


def api(method, path, **kwargs):
    try:
        r = getattr(requests, method)(f"{API}{path}", timeout=10, **kwargs)
        return r
    except requests.ConnectionError:
        return None


# ─── Auth ────────────────────────────────────────────────────────────────────

@app.route("/")
def index():
    return redirect(url_for("contests"))


@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        r = api("post", "/api/auth/login", json={
            "username": request.form["username"],
            "password": request.form["password"],
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 200:
            data = r.json()
            session["user_id"] = data["id"]
            session["username"] = data["username"]
            session["role"] = data["role"]
            return redirect(url_for("contests"))
        else:
            flash(r.json().get("error", "Неверный логин или пароль"), "error")
    return render_template("login.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        r = api("post", "/api/auth/register", json={
            "username": request.form["username"],
            "password": request.form["password"],
            "role": request.form["role"],
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 201:
            flash("Регистрация успешна. Войдите.", "success")
            return redirect(url_for("login"))
        else:
            flash(r.json().get("error", "Ошибка регистрации"), "error")
    return render_template("register.html")


@app.route("/logout")
def logout():
    session.clear()
    return redirect(url_for("login"))


# ─── Contests ────────────────────────────────────────────────────────────────

@app.route("/contests")
def contests():
    r = api("get", "/api/contests")
    items = r.json() if r and r.status_code == 200 else []
    return render_template("contests.html", contests=items)


@app.route("/contests/<int:cid>")
def contest(cid):
    r = api("get", f"/api/contests/{cid}")
    if not r or r.status_code != 200:
        flash("Контест не найден", "error")
        return redirect(url_for("contests"))
    c = r.json()

    problems = []
    for pid in c.get("problem_ids", []):
        pr = api("get", f"/api/problems/{pid}")
        if pr and pr.status_code == 200:
            problems.append(pr.json())
    return render_template("contest.html", contest=c, problems=problems)


# ─── Problems ────────────────────────────────────────────────────────────────

@app.route("/contests/<int:cid>/problems/<int:pid>", methods=["GET", "POST"])
def problem(cid, pid):
    pr = api("get", f"/api/problems/{pid}")
    if not pr or pr.status_code != 200:
        flash("Задача не найдена", "error")
        return redirect(url_for("contest", cid=cid))
    p = pr.json()

    if request.method == "POST":
        if "user_id" not in session:
            flash("Войдите, чтобы отправить решение", "error")
            return redirect(url_for("login"))

        code_file = request.files.get("code_file")
        code_text = request.form.get("code_text", "").strip()

        if code_file and code_file.filename:
            code = code_file.read().decode("utf-8", errors="replace")
        elif code_text:
            code = code_text
        else:
            flash("Прикрепите файл или вставьте код", "error")
            return render_template("problem.html", problem=p, contest_id=cid)

        r = api("post", f"/api/contests/{cid}/submit", json={
            "user_id": session["user_id"],
            "problem_id": pid,
            "code": code,
            "language": "cpp",
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 201:
            sub = r.json()
            return redirect(url_for("submission", sid=sub["id"]))
        else:
            flash(r.json().get("error", "Ошибка при отправке"), "error")

    return render_template("problem.html", problem=p, contest_id=cid)


# ─── Submissions ─────────────────────────────────────────────────────────────

@app.route("/submissions")
def submissions():
    if "user_id" not in session:
        flash("Войдите, чтобы увидеть посылки", "error")
        return redirect(url_for("login"))
    r = api("get", f"/api/users/{session['user_id']}/submissions")
    items = r.json() if r and r.status_code == 200 else []
    items = list(reversed(items))
    return render_template("submissions.html", submissions=items)


@app.route("/submissions/<int:sid>")
def submission(sid):
    r = api("get", f"/api/submissions/{sid}")
    if not r or r.status_code != 200:
        flash("Сабмит не найден", "error")
        return redirect(url_for("contests"))
    return render_template("submission.html", sub=r.json())


# ─── Admin: create contest/problem ───────────────────────────────────────────

@app.route("/problems/new", methods=["GET", "POST"])
def new_problem():
    if session.get("role") not in ("ADMIN", "JUDGE"):
        flash("Недостаточно прав", "error")
        return redirect(url_for("contests"))

    if request.method == "POST":
        test_inputs = request.form.getlist("test_input")
        test_outputs = request.form.getlist("test_output")
        test_cases = [{"input": i, "expected_output": o}
                      for i, o in zip(test_inputs, test_outputs) if i.strip()]
        r = api("post", "/api/problems", json={
            "actor_id": session["user_id"],
            "title": request.form["title"],
            "statement": request.form["statement"],
            "time_limit_ms": int(request.form.get("time_limit_ms", 2000)),
            "memory_limit_mb": int(request.form.get("memory_limit_mb", 256)),
            "test_cases": test_cases,
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 201:
            flash("Задача создана", "success")
            return redirect(url_for("problems_list"))
        else:
            flash(r.json().get("error", "Ошибка"), "error")
    return render_template("new_problem.html")


@app.route("/problems/<int:pid>", methods=["GET", "POST"])
def problem_standalone(pid):
    pr = api("get", f"/api/problems/{pid}")
    if not pr or pr.status_code != 200:
        flash("Задача не найдена", "error")
        return redirect(url_for("problems_list"))
    p = pr.json()

    rc = api("get", "/api/contests")
    all_contests = rc.json() if rc and rc.status_code == 200 else []
    available_contests = [c for c in all_contests if pid in c.get("problem_ids", [])]

    if request.method == "POST":
        if "user_id" not in session:
            flash("Войдите, чтобы отправить решение", "error")
            return redirect(url_for("login"))

        cid_str = request.form.get("contest_id", "")
        if not cid_str or not cid_str.isdigit():
            flash("Выберите контест", "error")
            return render_template("problem_standalone.html", problem=p, contests=available_contests)

        cid = int(cid_str)
        code_file = request.files.get("code_file")
        code_text = request.form.get("code_text", "").strip()

        if code_file and code_file.filename:
            code = code_file.read().decode("utf-8", errors="replace")
        elif code_text:
            code = code_text
        else:
            flash("Прикрепите файл или вставьте код", "error")
            return render_template("problem_standalone.html", problem=p, contests=available_contests)

        r = api("post", f"/api/contests/{cid}/submit", json={
            "user_id": session["user_id"],
            "problem_id": pid,
            "code": code,
            "language": "cpp",
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 201:
            return redirect(url_for("submission", sid=r.json()["id"]))
        else:
            flash(r.json().get("error", "Ошибка при отправке"), "error")

    return render_template("problem_standalone.html", problem=p, contests=available_contests)


@app.route("/problems")
def problems_list():
    r = api("get", "/api/problems")
    items = r.json() if r and r.status_code == 200 else []
    return render_template("problems.html", problems=items)


@app.route("/contests/new", methods=["GET", "POST"])
def new_contest():
    if session.get("role") != "ADMIN":
        flash("Только администратор может создавать контесты", "error")
        return redirect(url_for("contests"))

    if request.method == "POST":
        from datetime import datetime

        def parse_dt(s):
            return int(datetime.fromisoformat(s).timestamp())

        problem_ids = [int(x) for x in request.form.getlist("problem_ids") if x.isdigit()]

        r = api("post", "/api/contests", json={
            "actor_id": session["user_id"],
            "title": request.form["title"],
            "start_time": parse_dt(request.form["start_time"]),
            "end_time": parse_dt(request.form["end_time"]),
            "problem_ids": problem_ids,
        })
        if r is None:
            flash("Сервер недоступен", "error")
        elif r.status_code == 201:
            flash("Контест создан", "success")
            return redirect(url_for("contests"))
        else:
            flash(r.json().get("error", "Ошибка"), "error")

    r = api("get", "/api/problems")
    all_problems = r.json() if r and r.status_code == 200 else []
    return render_template("new_contest.html", problems=all_problems)


if __name__ == "__main__":
    port = int(os.environ.get("CF_WEB_PORT", 5000))
    app.run(debug=True, port=port)
