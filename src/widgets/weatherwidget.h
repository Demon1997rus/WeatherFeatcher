#pragma once

#include <QPointF>
#include <QWidget>

#include "structures/weatherinfo.h"
#include "utils/citydirectory.h"

class CitySearchBox;
class QLabel;
class QTimer;
class WeatherCard;

// Top-level window: composes CitySearchBox, WeatherCard and a status line,
// owns the periodic refresh timer. Single outward signal: requestWeatherData.
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

private:
    void setupLayout();
    void initConnects();
    void applyStyles();
    void onCitySelected(const CityDirectory::Entry &entry);

    CitySearchBox *searchBox = nullptr;
    WeatherCard *card = nullptr;
    QLabel *statusLabel = nullptr;
    QTimer *refreshTimer = nullptr;

    QPointF currentCoordinates;
    bool hasSelection = false;
};
