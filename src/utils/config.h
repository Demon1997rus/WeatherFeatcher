#pragma once

#include <QString>
#include <QtGlobal>

// Application-wide settings. Currently just the OpenWeatherMap API key,
// taken from the OPENWEATHER_API_KEY environment variable at startup.
class Config {
public:
    static Config &instance();

    const QString &apiKey() const;

private:
    Q_DISABLE_COPY(Config)
    Config();

    QString apiKey_;
};
