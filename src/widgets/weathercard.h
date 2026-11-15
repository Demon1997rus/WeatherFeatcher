#pragma once

#include <QWidget>

#include "structures/weatherinfo.h"

class QLabel;

// Presentational card: renders WeatherInfo, knows nothing about networking
// or city search.
class WeatherCard : public QWidget {
    Q_OBJECT
public:
    explicit WeatherCard(QWidget *parent = nullptr);

public slots:
    void displayWeatherData(const WeatherInfo &info);
    void setCityName(const QString &cityName);
    void clearData();

private:
    QLabel *createValueLabel();
    void setupLayout();
    void applyStyles();
    void setDefaultValues();

    static QString iconEmoji(const QString &iconCode);
    static QString formatTemperature(double temperature);
    static QString windDirectionText(int degrees);

    QLabel *iconLabel = nullptr;
    QLabel *cityLabel = nullptr;
    QLabel *timeLabel = nullptr;
    QLabel *temperatureLabel = nullptr;
    QLabel *feelsLikeLabel = nullptr;
    QLabel *descriptionLabel = nullptr;
    QLabel *humidityValue = nullptr;
    QLabel *pressureValue = nullptr;
    QLabel *windSpeedValue = nullptr;
    QLabel *windDirectionValue = nullptr;
    QLabel *cloudinessValue = nullptr;
};
