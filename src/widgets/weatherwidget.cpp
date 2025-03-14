#include "weatherwidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPair>

#include "utils/config.h"

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
    temperatureLabel->setText(QString("Температура: %1 °C").arg(info.getTemperature()));
    feelsLikeLabel->setText(QString("Ощущается как: %1 °C").arg(info.getFeelsLike()));
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
    QMessageBox::critical(this, "Ошибка", errorMessage);
}

void WeatherWidget::onCitySelected(int index)
{
    auto coordinates = cbCities->itemData(index).value<QPair<double, double>>();
    emit requestWeatherData(coordinates.first, coordinates.second);
}

void WeatherWidget::initWidgets()
{
    mainLayout = new QGridLayout(this);
    cbCities = new QComboBox(this);
    searchField = new QLineEdit(this);
    temperatureLabel = new QLabel(this);
    feelsLikeLabel = new QLabel(this);
    descriptionLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    pressureLabel = new QLabel(this);
    windSpeedLabel = new QLabel(this);
    windDirectionLabel = new QLabel(this);
    cloudinessLabel = new QLabel(this);
}

void WeatherWidget::settingsWidgets()
{
    // Настройка комбобокса со списком городов
    for (const auto &city : Config::instance().coordinatesCities()) {
        cbCities->addItem(
            QString::fromStdString(city.getCity()),
            QVariant::fromValue(qMakePair(city.getLatitude(), city.getLongitude())));
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
    mainLayout->addWidget(temperatureLabel);
    mainLayout->addWidget(feelsLikeLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(humidityLabel);
    mainLayout->addWidget(pressureLabel);
    mainLayout->addWidget(windSpeedLabel);
    mainLayout->addWidget(windDirectionLabel);
    mainLayout->addWidget(cloudinessLabel);

    mainLayout->setRowStretch(mainLayout->count(), 1);
}

void WeatherWidget::initConnects()
{
    connect(cbCities, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &WeatherWidget::onCitySelected);
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
}

void WeatherWidget::dynamicBackground()
{
    QPixmap background("../data/background.jpg");
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
