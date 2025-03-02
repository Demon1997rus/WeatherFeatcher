#include "weatherinfo.h"

WeatherInfo::WeatherInfo(BOOST_COPY_ASSIGN_REF(WeatherInfo) other) :
    temperature(other.temperature), feelsLike(other.feelsLike),
    weatherDescription(other.weatherDescription), humidity(other.humidity),
    pressure(other.pressure), windSpeed(other.windSpeed), windDirection(other.windDirection),
    cloudiness(other.cloudiness)
{
}

WeatherInfo::WeatherInfo(BOOST_RV_REF(WeatherInfo) other) BOOST_NOEXCEPT
{
    this->swap(other);
}

WeatherInfo &WeatherInfo::operator=(BOOST_COPY_ASSIGN_REF(WeatherInfo) other)
{
    WeatherInfo temp(other);
    this->swap(temp);
    return *this;
}

WeatherInfo &WeatherInfo::operator=(BOOST_RV_REF(WeatherInfo) other) BOOST_NOEXCEPT
{
    WeatherInfo temp(boost::move(other));
    this->swap(temp);
    return *this;
}

void WeatherInfo::swap(WeatherInfo &rhs) BOOST_NOEXCEPT
{
    boost::swap(this->temperature, rhs.temperature);
    boost::swap(this->feelsLike, rhs.feelsLike);
    this->weatherDescription.swap(rhs.weatherDescription);
    boost::swap(this->humidity, rhs.humidity);
    boost::swap(this->pressure, rhs.pressure);
    boost::swap(this->windSpeed, rhs.windSpeed);
    boost::swap(this->windDirection, rhs.windDirection);
    boost::swap(this->cloudiness, rhs.cloudiness);
}

void WeatherInfo::setTemperature(const double temp)
{
    this->temperature = temp;
}

void WeatherInfo::setFeelsLike(const double feelsLikeTemp)
{
    this->feelsLike = feelsLikeTemp;
}

void WeatherInfo::setWeatherDescription(BOOST_COPY_ASSIGN_REF(QString) description)
{
    this->weatherDescription = description;
}

void WeatherInfo::setWeatherDescription(BOOST_RV_REF(QString) description) BOOST_NOEXCEPT
{
    this->weatherDescription = boost::move(description);
}

void WeatherInfo::setHumidity(const int hum)
{
    this->humidity = hum;
}

void WeatherInfo::setPressure(const int press)
{
    this->pressure = press;
}

void WeatherInfo::setWindSpeed(const double speed)
{
    this->windSpeed = speed;
}

void WeatherInfo::setWindDirection(const int direction)
{
    this->windDirection = direction;
}

void WeatherInfo::setCloudiness(const int clouds)
{
    this->cloudiness = clouds;
}

double WeatherInfo::getTemperature() const
{
    return this->temperature;
}

double WeatherInfo::getFeelsLike() const
{
    return this->feelsLike;
}

QString &WeatherInfo::getWeatherDescription()
{
    return this->weatherDescription;
}

BOOST_COPY_ASSIGN_REF(QString) WeatherInfo::getWeatherDescription() const
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
