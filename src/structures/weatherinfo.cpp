#include "weatherinfo.h"

void WeatherInfo::setTemperature(const double temperature)
{
    this->temperature = temperature;
}

void WeatherInfo::setFeelsLike(const double feelsLike)
{
    this->feelsLike = feelsLike;
}

void WeatherInfo::setWeatherDescription(const QString &description)
{
    this->weatherDescription = description;
}

void WeatherInfo::setWeatherDescription(QString &&description)
{
    this->weatherDescription = std::move(description);
}

void WeatherInfo::setHumidity(const int humidity)
{
    this->humidity = humidity;
}

void WeatherInfo::setPressure(const int pressure)
{
    this->pressure = pressure;
}

void WeatherInfo::setWindSpeed(const double speed)
{
    this->windSpeed = speed;
}

void WeatherInfo::setWindDirection(const int direction)
{
    this->windDirection = direction;
}

void WeatherInfo::setCloudiness(const int cloudiness)
{
    this->cloudiness = cloudiness;
}

void WeatherInfo::setIconCode(const QString &iconCode)
{
    this->iconCode = iconCode;
}

double WeatherInfo::getTemperature() const
{
    return this->temperature;
}

double WeatherInfo::getFeelsLike() const
{
    return this->feelsLike;
}

const QString &WeatherInfo::getWeatherDescription() const
{
    return this->weatherDescription;
}

int WeatherInfo::getHumidity() const
{
    return this->humidity;
}

int WeatherInfo::getPressure() const
{
    return this->pressure;
}

double WeatherInfo::getWindSpeed() const
{
    return this->windSpeed;
}

int WeatherInfo::getWindDirection() const
{
    return this->windDirection;
}

int WeatherInfo::getCloudiness() const
{
    return this->cloudiness;
}

const QString &WeatherInfo::getIconCode() const
{
    return this->iconCode;
}
