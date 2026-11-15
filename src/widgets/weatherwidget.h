#pragma once

#include <QComboBox>
#include <QGridLayout>
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
    void initWidgets();
    void settingsWidgets();
    void setupLayout();
    void initConnects();
    void setDefaultValues();
    void applyStyles();
    void dynamicBackground();
    void updateWeatherIcon(const QString &iconCode);

private:
    QGridLayout *mainLayout = nullptr;
    QComboBox *cbCities = nullptr;
    QLineEdit *searchField = nullptr;
    QLabel *iconLabel = nullptr;
    QLabel *temperatureLabel = nullptr;
    QLabel *feelsLikeLabel = nullptr;
    QLabel *descriptionLabel = nullptr;
    QLabel *humidityLabel = nullptr;
    QLabel *pressureLabel = nullptr;
    QLabel *windSpeedLabel = nullptr;
    QLabel *windDirectionLabel = nullptr;
    QLabel *cloudinessLabel = nullptr;
    QLabel *statusLabel = nullptr;

    QTimer *refreshTimer = nullptr;
    QNetworkAccessManager *iconDownloader = nullptr;
    QHash<QString, QPixmap> iconCache;

protected:
    void resizeEvent(QResizeEvent *event) override;
};
