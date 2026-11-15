#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QPointF>
#include <QWidget>

#include "structures/weatherinfo.h"
#include "utils/citydirectory.h"

class QNetworkAccessManager;
class QListWidget;
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
    void performSearch();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *createValueLabel();
    void initWidgets();
    void setupLayout();
    void initConnects();
    void setDefaultValues();
    void applyStyles();
    void selectCity(const CityDirectory::Entry &entry);
    void showBrowseList();
    void showResults(const std::vector<CityDirectory::Entry> &results);
    void hideSearchPopup();

    static QString iconEmoji(const QString &iconCode);
    static QString formatTemperature(double temperature);
    static QString windDirectionText(int degrees);

private:
    const CityDirectory &cities;

    QLineEdit *searchField = nullptr;
    QListWidget *searchPopup = nullptr;
    QTimer *searchDebounce = nullptr;
    bool suppressSearch = false;

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
    QLabel *statusLabel = nullptr;

    QPointF currentCoordinates;
    bool hasSelection = false;

    QTimer *refreshTimer = nullptr;
};
