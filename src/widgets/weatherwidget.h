#pragma once

#include <QComboBox>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QWidget>

#include "structures/weatherinfo.h"

class QNetworkAccessManager;
class QTimer;

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
    QLabel *createValueLabel();
    void initWidgets();
    void setupLayout();
    void initConnects();
    void setDefaultValues();
    void applyStyles();
    void updateWeatherIcon(const QString &iconCode);

    static QString formatTemperature(double temperature);
    static QString windDirectionText(int degrees);

private:
    QComboBox *cbCities = nullptr;
    QLineEdit *searchField = nullptr;
    QLabel *iconLabel = nullptr;
    QLabel *temperatureLabel = nullptr;
    QLabel *feelsLikeLabel = nullptr;
    QLabel *descriptionLabel = nullptr;
    QLabel *humidityValue = nullptr;
    QLabel *pressureValue = nullptr;
    QLabel *windSpeedValue = nullptr;
    QLabel *windDirectionValue = nullptr;
    QLabel *cloudinessValue = nullptr;
    QLabel *statusLabel = nullptr;

    QTimer *refreshTimer = nullptr;
    QNetworkAccessManager *iconDownloader = nullptr;
    QHash<QString, QPixmap> iconCache;
};
