#include "config.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Config &Config::instance()
{
    static Config instance;
    return instance;
}

bool Config::loadConfig(const QString &configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << Q_FUNC_INFO;
        qCritical() << file.errorString();
        return false;
    }

    // загрузка переменных окружения
    api_key = qgetenv("OPENWEATHER_API_KEY").toStdString();
    if (api_key.empty()) {
        qCritical() << Q_FUNC_INFO;
        qCritical() << "Missing \"OPENWEATHER_API_KEY\" in environment";
        return false;
    }

    // загрузка json данных
    QJsonParseError jsonError;
    const QJsonDocument configDoc = QJsonDocument::fromJson(file.readAll(), &jsonError);
    if (jsonError.error != jsonError.NoError) {
        qCritical() << Q_FUNC_INFO;
        qCritical() << jsonError.errorString();
        return false;
    }

    // Получение данных для городов
    const QJsonObject configJson = configDoc.object();
    const QJsonArray arrayCity = configJson["cities"].toArray();
    if (arrayCity.empty()) {
        qCritical() << Q_FUNC_INFO;
        qCritical() << "At least one city must be specified in the configuration";
        return false;
    }

    coordinates_cities.reserve(arrayCity.size());
    for (const auto cityValue : arrayCity) {
        const QJsonObject cityData = cityValue.toObject();
        CityCoordinate city;
        city.setCity(cityData["name"].toString().toStdString());
        city.setLatitude(cityData["latitude"].toDouble());
        city.setLongitude(cityData["longitude"].toDouble());
        coordinates_cities.emplace_back(qMove(city));
    }

    return true;
}

const std::string &Config::apiKey() const
{
    return this->api_key;
}

const std::vector<CityCoordinate> &Config::coordinatesCities() const
{
    return this->coordinates_cities;
}
