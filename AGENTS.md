# AGENTS.md — WeatherFeatcher

## Проект
Qt 6 / C++17 десктоп-приложение погоды (OpenWeatherMap). Pet-проект, кросс-платформенный
(Windows/Linux), сборка через CMake Presets. Поиск города — офлайн, по локальной базе
GeoNames (~34 тыс. городов, `data/cities.json`), Geocoding API не используется.

## Команды
- Сборка Windows (MSVC, Qt в D:/programs/Qt/6.12.0/msvc2022_64):
  `cmake --preset windows-debug && cmake --build --preset windows-debug`
  `cmake --preset windows-release && cmake --build --preset windows-release`
- Сборка Linux: пресеты `linux-debug` / `linux-release` (Qt 6 из системы).
- Запуск: бинарь в `build/windows/src/<Debug|Release>/weather-fetcher.exe`.
  Данные (`data/`) копируются рядом с бинарём при сборке — запуск из любой папки.
- Требуется переменная окружения `OPENWEATHER_API_KEY`, иначе приложение падает при старте.
- Тесты: Qt Test в `tests/`. Прогон (после сборки Debug):
  `ctest --test-dir build/windows -C Debug --output-on-failure`
- Скрытая опция `--screenshot <file.png>`: рендерит виджет с sample-данными в PNG
  (проверка дизайна без ручного запуска; API-ключ не нужен).
- Обновление базы городов: скачать cities15000.txt + alternateNamesV2.txt с
  download.geonames.org/export/dump/ и прогнать
  `python tools/build_cities_json.py cities15000.txt data/cities.json alternateNamesV2.txt`

## Правила стиля
- C++17, Qt 6. Заголовки — `#pragma once`. Слоты/сигналы — `Q_OBJECT` + connect с указателями на члены.
- Комментарии и документация — по-английски. UI-строки — по-русски (интерфейс русский).
- Qt-виджеты создавать через `new` с parent'ом — НЕ заворачивать в smart pointers.
- Plain-data структуры: инициализировать ВСЕ поля (NSDMI), rule-of-zero, без Boost.
- Новые зависимости — только через `find_package` в CMakeLists.
- Исходники перечислять явно в src/CMakeLists.txt — `file(GLOB)` запрещён.
- Название города на карточке — то, что выбрал пользователь (из локальной базы);
  имя из поля "name" ответа API не показывать (это ближайшая метеоточка).

## Запреты
- НЕ коммитить API-ключи/токены. Ключ — только через `OPENWEATHER_API_KEY`.
- НЕ делать блокирующих вызовов (сеть, файлы) в GUI-потоке. Сеть — QNetworkAccessManager,
  таймаут обязателен (см. src/services/httpservice.cpp).
- НЕ использовать API, удалённые/deprecated в Qt 6 (qMove, QDesktopWidget, ...).

## Требует approve перед изменением
- `data/cities.json` — генерируется скриптом, вручную НЕ редактировать.
- `src/utils/config.*` (API-ключ из окружения).
- Смена стандарта C++, мажорной версии Qt, добавление CI-инфраструктуры.

## Известные долги (не чинить без явного запроса)
- Нет прогноза на 5 дней и запоминания последнего города (QSettings).
