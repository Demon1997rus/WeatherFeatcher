#pragma once

#include <QString>

#include <boost/move/move.hpp>
#include <boost/swap.hpp>

class WeatherInfo {
private:
    BOOST_COPYABLE_AND_MOVABLE(WeatherInfo)

public:
    WeatherInfo() = default;

    WeatherInfo(BOOST_COPY_ASSIGN_REF(WeatherInfo) other);

    WeatherInfo(BOOST_RV_REF(WeatherInfo) other) BOOST_NOEXCEPT;

    WeatherInfo &operator=(BOOST_COPY_ASSIGN_REF(WeatherInfo) other);

    WeatherInfo &operator=(BOOST_RV_REF(WeatherInfo) other) BOOST_NOEXCEPT;

    void swap(WeatherInfo &rhs) BOOST_NOEXCEPT;

    void setTemperature(const double temperature);
    void setFeelsLike(const double feelsLike);
    void setWeatherDescription(BOOST_COPY_ASSIGN_REF(QString) description);
    void setWeatherDescription(BOOST_RV_REF(QString) description) BOOST_NOEXCEPT;
    void setHumidity(const int humidity);
    void setPressure(const int pressure);
    void setWindSpeed(const double speed);
    void setWindDirection(const int direction);
    void setCloudiness(const int cloudiness);

    double getTemperature() const;
    double getFeelsLike() const;
    QString &getWeatherDescription();
    BOOST_COPY_ASSIGN_REF(QString) getWeatherDescription() const;
    int getHumidity() const;
    int getPressure() const;
    double getWindSpeed() const;
    int getWindDirection() const;
    int getCloudiness() const;

private:
    double temperature; // Текущая температура воздуха в градусах Цельсия
    double feelsLike; // Ощущаемая температура в градусах Цельсия
    QString weatherDescription; // Краткое текстовое описание текущих погодных условий
    int humidity;               // Влажность воздуха в процентах
    int pressure; // Атмосферное давление в гектопаскалях (гПа)
    double windSpeed;  // Скорость ветра в метрах в секунду (м/с)
    int windDirection; // Направление ветра в градусах (0° - север, 90° - восток и т.д.)
    int cloudiness; // Облачность в процентах (процент покрытия неба облаками)
};
