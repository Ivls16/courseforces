# CourseForces

## Сборка

```bash
git clone https://github.com/Ivls16/courseforces.git
cd courseforces

cmake -S . -B build \
      -DCMAKE_CXX_COMPILER=g++-13 \
      -DCMAKE_C_COMPILER=gcc-13

cmake --build build -j$(nproc)
```

## Запуск сервера

```bash
./build/courseforces_server
```

## Запуск веб-интерфейса

```bash
pip install -r web/requirements.txt
./build/courseforces_server &
python3 web/app.py
```

Открыть в браузере: http://localhost:5000

## Тесты

```bash
cd build && ctest --output-on-failure
```
