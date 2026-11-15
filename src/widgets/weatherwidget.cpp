#include "weatherwidget.h"

#include <QApplication>
#include <QCoreApplication>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointF>
#include <QTimer>

#include "utils/config.h"

namespace {
constexpr int refreshIntervalMs = 10 * 60 * 1000; // Refresh weather every 10 minutes
}

WeatherWidget::WeatherWidget(QWidget *parent) : QWidget(parent)
{
    initWidgets();
    settingsWidgets();
    setupLayout();
    initConnects();
    applyStyles();
}

void WeatherWidget::loadCurrentCityWeather()
{
    onCitySelected(cbCities->currentIndex());
}

void WeatherWidget::displayWeatherData(const WeatherInfo &info)
{
    // Temperature with one decimal and an explicit sign, e.g. "+3.3"
    const auto formatTemp = [](double temperature) {
        return QString("%1%2").arg(temperature > 0 ? "+" : "").arg(temperature, 0, 'f', 1);
    };

    statusLabel->clear();
    updateWeatherIcon(info.getIconCode());

    temperatureLabel->setText(QString("Температура: %1 °C").arg(formatTemp(info.getTemperature())));
    feelsLikeLabel->setText(QString("Ощущается как: %1 °C").arg(formatTemp(info.getFeelsLike())));
    descriptionLabel->setText(QString("Описание: %1").arg(info.getWeatherDescription()));
    humidityLabel->setText(QString("Влажность: %1%").arg(info.getHumidity()));
    pressureLabel->setText(QString("Давление: %1 hPa").arg(info.getPressure()));
    windSpeedLabel->setText(QString("Скорость ветра: %1 м/с").arg(info.getWindSpeed()));
    windDirectionLabel->setText(QString("Направление ветра: %1°").arg(info.getWindDirection()));
    cloudinessLabel->setText(QString("Облачность: %1%").arg(info.getCloudiness()));
}

void WeatherWidget::displayError(const QString &errorMessage)
{
    setDefaultValues();
    // Non-modal error reporting: a status line keeps the UI usable
    statusLabel->setText(errorMessage);
}

void WeatherWidget::onCitySelected(int index)
{
    const QPointF coordinates = cbCities->itemData(index).toPointF();
    emit requestWeatherData(coordinates.x(), coordinates.y());
}

void WeatherWidget::initWidgets()
{
    mainLayout = new QGridLayout(this);
    cbCities = new QComboBox(this);
    searchField = new QLineEdit(this);
    iconLabel = new QLabel(this);
    temperatureLabel = new QLabel(this);
    feelsLikeLabel = new QLabel(this);
    descriptionLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    pressureLabel = new QLabel(this);
    windSpeedLabel = new QLabel(this);
    windDirectionLabel = new QLabel(this);
    cloudinessLabel = new QLabel(this);
    statusLabel = new QLabel(this);

    refreshTimer = new QTimer(this);
    iconDownloader = new QNetworkAccessManager(this);
}

void WeatherWidget::settingsWidgets()
{
    // Настройка комбобокса со списком городов
    for (const auto &city : Config::instance().coordinatesCities()) {
        cbCities->addItem(QString::fromStdString(city.getCity()),
                          QPointF(city.getLatitude(), city.getLongitude()));
    }
    searchField->setPlaceholderText("Поиск города...");
    cbCities->setEditable(true);
    cbCities->setLineEdit(searchField);

    // Установка дефолтных значения для виджетов
    setDefaultValues();
}

void WeatherWidget::setupLayout()
{
    mainLayout->addWidget(cbCities);
    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(temperatureLabel);
    mainLayout->addWidget(feelsLikeLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(humidityLabel);
    mainLayout->addWidget(pressureLabel);
    mainLayout->addWidget(windSpeedLabel);
    mainLayout->addWidget(windDirectionLabel);
    mainLayout->addWidget(cloudinessLabel);
    mainLayout->addWidget(statusLabel);

    mainLayout->setRowStretch(mainLayout->count(), 1);
}

void WeatherWidget::initConnects()
{
    connect(cbCities, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &WeatherWidget::onCitySelected);

    // Periodic refresh of the currently selected city
    connect(refreshTimer, &QTimer::timeout, this, &WeatherWidget::loadCurrentCityWeather);
    refreshTimer->start(refreshIntervalMs);
}

void WeatherWidget::setDefaultValues()
{
    temperatureLabel->setText("Температура: -- °C");
    feelsLikeLabel->setText("Ощущается как: -- °C");
    descriptionLabel->setText("Описание: --");
    humidityLabel->setText("Влажность: --%");
    pressureLabel->setText("Давление: -- hPa");
    windSpeedLabel->setText("Скорость ветра: -- м/с");
    windDirectionLabel->setText("Направление ветра: --°");
    cloudinessLabel->setText("Облачность: --%");
}

void WeatherWidget::applyStyles()
{
    dynamicBackground();

    QString labelStyle = "QLabel { color: white; }";

    temperatureLabel->setStyleSheet(labelStyle);
    feelsLikeLabel->setStyleSheet(labelStyle);
    descriptionLabel->setStyleSheet(labelStyle);
    humidityLabel->setStyleSheet(labelStyle);
    pressureLabel->setStyleSheet(labelStyle);
    windSpeedLabel->setStyleSheet(labelStyle);
    windDirectionLabel->setStyleSheet(labelStyle);
    cloudinessLabel->setStyleSheet(labelStyle);
    statusLabel->setStyleSheet("QLabel { color: #ff6666; }");
}

void WeatherWidget::updateWeatherIcon(const QString &iconCode)
{
    if (iconCode.isEmpty()) {
        iconLabel->clear();
        return;
    }

    const auto cachedIcon = iconCache.constFind(iconCode);
    if (cachedIcon != iconCache.constEnd()) {
        iconLabel->setPixmap(cachedIcon.value());
        return;
    }

    // Icon set: https://openweathermap.org/weather-conditions
    const QUrl iconUrl(QString("https://openweathermap.org/img/wn/%1@2x.png").arg(iconCode));
    QNetworkReply *reply = iconDownloader->get(QNetworkRequest(iconUrl));
    connect(reply, &QNetworkReply::finished, this, [this, reply, iconCode]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        const QPixmap icon = QPixmap::fromImage(QImage::fromData(reply->readAll()));
        if (icon.isNull()) {
            return;
        }
        iconCache.insert(iconCode, icon);
        iconLabel->setPixmap(icon);
    });
}

void WeatherWidget::dynamicBackground()
{
    // The background image is copied next to the executable at build time
    QPixmap background(QCoreApplication::applicationDirPath() + "/data/background.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, background);
    this->setPalette(palette);
}

void WeatherWidget::resizeEvent(QResizeEvent *event)
{
    dynamicBackground();
    QWidget::resizeEvent(event);
}
