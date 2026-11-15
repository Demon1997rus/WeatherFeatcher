# WeatherFeatcher

Приложение на Qt 6 для отслеживания актуальной погоды (OpenWeatherMap).

## Стек

- C++17, Qt 6 (Widgets, Network)
- CMake 3.21+ с CMakePresets
- Асинхронные запросы через QNetworkAccessManager

## Сборка

### Windows (MSVC)

Потребуется Qt 6 (путь по умолчанию в пресете: `D:/programs/Qt/6.12.0/msvc2022_64`,
при другом расположении поправьте `CMAKE_PREFIX_PATH` в `CMakePresets.json`):

```bash
cmake --preset windows-debug
cmake --build --preset windows-debug
```

### Linux

```bash
cmake --preset linux-release
cmake --build --preset linux-release
```

## Запуск

1. Получите API-ключ на https://openweathermap.org (бесплатно) и задайте
   переменную окружения `OPENWEATHER_API_KEY`.
2. Запустите бинарь из `build/windows/src/<Debug|Release>/` (Windows)
   или `build/linux-release/` (Linux). Папка `data/` копируется рядом
   с бинарём автоматически при сборке.

Список городов — в `data/config.json`.
