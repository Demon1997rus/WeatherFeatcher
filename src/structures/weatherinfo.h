#pragma once

#include <QString>

class WeatherInfo {
public:
    void setTemperature(double temperature);
    void setFeelsLike(double feelsLike);
    void setWeatherDescription(const QString &description);
    void setWeatherDescription(QString &&description);
    void setHumidity(int humidity);
    void setPressure(int pressure);
    void setWindSpeed(double speed);
    void setWindDirection(int direction);
    void setCloudiness(int cloudiness);
    void setIconCode(const QString &iconCode);
    void setCityName(const QString &cityName);

    double getTemperature() const;
    double getFeelsLike() const;
    const QString &getWeatherDescription() const;
    int getHumidity() const;
    int getPressure() const;
    double getWindSpeed() const;
    int getWindDirection() const;
    int getCloudiness() const;
    const QString &getIconCode() const;
    const QString &getCityName() const;

private:
    double temperature = 0.0;     // Current air temperature in Celsius
    double feelsLike = 0.0;       // Perceived temperature in Celsius
    QString weatherDescription;   // Short text description of current weather conditions
    int humidity = 0;             // Air humidity in percent
    int pressure = 0;             // Atmospheric pressure in hectopascals (hPa)
    double windSpeed = 0.0;       // Wind speed in meters per second (m/s)
    int windDirection = 0;        // Wind direction in degrees (0 - north, 90 - east, etc.)
    int cloudiness = 0;           // Cloudiness in percent (sky coverage)
    QString iconCode;             // OpenWeatherMap icon code (e.g. "10d")
    QString cityName;             // City name reported by the API
};
