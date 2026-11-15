#pragma once

#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPointF>
#include <QWidget>

#include "structures/weatherinfo.h"
#include "utils/citydirectory.h"

class QCompleter;
class QNetworkAccessManager;
class QStringListModel;
class QTimer;

class WeatherWidget : public QWidget {
    Q_OBJECT
public:
    explicit WeatherWidget(const CityDirectory &cities, QWidget *parent = nullptr);

    void loadCurrentCityWeather();

public slots:
    void displayWeatherData(const WeatherInfo &info);
    void displayError(const QString &errorMessage);

signals:
    void requestWeatherData(double latitude, double longitude);

private slots:
    void onCityActivated(const QString &displayName);
    void onSearchAccepted();

private:
    QLabel *createValueLabel();
    void initWidgets();
    void setupLayout();
    void initConnects();
    void setDefaultValues();
    void applyStyles();
    void selectCity(const CityDirectory::Entry &entry);
    void updateWeatherIcon(const QString &iconCode);

    static QString formatTemperature(double temperature);
    static QString windDirectionText(int degrees);

private:
    const CityDirectory &cities;

    QLineEdit *searchField = nullptr;
    QStringListModel *cityModel = nullptr;
    QCompleter *cityCompleter = nullptr;

    QLabel *iconLabel = nullptr;
    QLabel *cityLabel = nullptr;
    QLabel *temperatureLabel = nullptr;
    QLabel *feelsLikeLabel = nullptr;
    QLabel *descriptionLabel = nullptr;
    QLabel *humidityValue = nullptr;
    QLabel *pressureValue = nullptr;
    QLabel *windSpeedValue = nullptr;
    QLabel *windDirectionValue = nullptr;
    QLabel *cloudinessValue = nullptr;
    QLabel *statusLabel = nullptr;

    QPointF currentCoordinates;
    bool hasSelection = false;

    QTimer *refreshTimer = nullptr;
    QNetworkAccessManager *iconDownloader = nullptr;
    QHash<QString, QPixmap> iconCache;
};
