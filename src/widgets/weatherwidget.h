#pragma once

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "structures/weatherinfo.h"

class WeatherWidget : public QWidget {
    Q_OBJECT
public:
    explicit WeatherWidget(QWidget *parent = nullptr);

    void loadCurrentCityWeather();

public slots:
    void displayWeatherData(const WeatherInfo &info);
    void displayError(const QString &errorMessage);

signals:
    void requestWeatherData(double latitude, double longitude);

private slots:
    void onCitySelected(int index);

private:
    void initWidgets();
    void settingsWidgets();
    void setupLayout();
    void initConnects();
    void setDefaultValues();

private:
    QGridLayout *mainLayout = nullptr;
    QComboBox *cbCities = nullptr;
    QLineEdit *searchField = nullptr;
    QLabel *temperatureLabel = nullptr;
    QLabel *feelsLikeLabel = nullptr;
    QLabel *descriptionLabel = nullptr;
    QLabel *humidityLabel = nullptr;
    QLabel *pressureLabel = nullptr;
    QLabel *windSpeedLabel = nullptr;
    QLabel *windDirectionLabel = nullptr;
    QLabel *cloudinessLabel = nullptr;
};
