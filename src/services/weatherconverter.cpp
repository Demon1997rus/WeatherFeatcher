#include "weatherconverter.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

WeatherConverter::WeatherConverter(QObject *parent) : QObject(parent) {}

void WeatherConverter::convertToWeatherInfo(const QByteArray &data) const
{
    QJsonParseError jsonError;
    const QJsonDocument weatherDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != jsonError.NoError) {
        qCritical() << Q_FUNC_INFO;
        qCritical() << jsonError.errorString();
        return;
    }
    const QJsonObject weatherObject = weatherDoc.object();

    WeatherInfo weatherInfo;

    // Извлечение данных из ключа "main"
    if (weatherObject.contains("main") && weatherObject["main"].isObject()) {
        const QJsonObject mainObj = weatherObject["main"].toObject();
        weatherInfo.setTemperature(mainObj["temp"].toDouble() - 273.15);
        weatherInfo.setFeelsLike(mainObj["feels_like"].toDouble() - 273.15);
        weatherInfo.setHumidity(mainObj["humidity"].toInt());
        weatherInfo.setPressure(mainObj["pressure"].toInt());
    }

    // Извлечение данных из массива "weather"
    if (weatherObject.contains("weather") && weatherObject["weather"].isArray()) {
        const QJsonArray weatherArray = weatherObject["weather"].toArray();
        if (!weatherArray.isEmpty()) {
            const QJsonObject weatherDetails = weatherArray.first().toObject();
            weatherInfo.setWeatherDescription(weatherDetails["description"].toString());
        }
    }

    // Извлечение данных из ключа "wind"
    if (weatherObject.contains("wind") && weatherObject["wind"].isObject()) {
        const QJsonObject windObj = weatherObject["wind"].toObject();
        weatherInfo.setWindSpeed(windObj["speed"].toDouble());
        weatherInfo.setWindDirection(windObj["deg"].toInt());
    }

    // Извлечение данных из ключа "clouds"
    if (weatherObject.contains("clouds") && weatherObject["clouds"].isObject()) {
        const QJsonObject cloudsObj = weatherObject["clouds"].toObject();
        weatherInfo.setCloudiness(cloudsObj["all"].toInt());
    }

    emit weatherInfoReady(weatherInfo);
}
