#include "weathercard.h"

#include <QDateTime>
#include <QGridLayout>
#include <QHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

WeatherCard::WeatherCard(QWidget *parent) : QWidget(parent)
{
    setObjectName("card");
    // A QWidget subclass ignores QSS background/border unless asked to paint
    // them (a plain QWidget does this implicitly)
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(520);

    iconLabel = new QLabel(this);
    iconLabel->setObjectName("iconEmoji");
    iconLabel->setAlignment(Qt::AlignCenter);
    cityLabel = new QLabel(this);
    cityLabel->setObjectName("city");
    cityLabel->setAlignment(Qt::AlignCenter);
    timeLabel = new QLabel(this);
    timeLabel->setObjectName("secondary");
    timeLabel->setAlignment(Qt::AlignCenter);
    temperatureLabel = new QLabel(this);
    temperatureLabel->setObjectName("temperature");
    feelsLikeLabel = new QLabel(this);
    feelsLikeLabel->setObjectName("secondary");
    descriptionLabel = new QLabel(this);
    descriptionLabel->setObjectName("secondary");

    humidityValue = createValueLabel();
    pressureValue = createValueLabel();
    windSpeedValue = createValueLabel();
    windDirectionValue = createValueLabel();
    cloudinessValue = createValueLabel();

    setupLayout();
    setDefaultValues();
    applyStyles();
}

void WeatherCard::displayWeatherData(const WeatherInfo &info)
{
    iconLabel->setText(iconEmoji(info.getIconCode()));

    // Local time of the city from its UTC offset
    timeLabel->setText(QString("Местное время: %1")
                           .arg(QDateTime::currentDateTimeUtc()
                                    .addSecs(info.getTimezoneOffset())
                                    .toString("HH:mm")));

    temperatureLabel->setText(formatTemperature(info.getTemperature()));
    feelsLikeLabel->setText(QString("Ощущается как %1").arg(formatTemperature(info.getFeelsLike())));
    descriptionLabel->setText(info.getWeatherDescription());
    humidityValue->setText(QString("%1%").arg(info.getHumidity()));
    pressureValue->setText(QString("%1 hPa").arg(info.getPressure()));
    windSpeedValue->setText(QString("%1 м/с").arg(info.getWindSpeed(), 0, 'f', 1));
    windDirectionValue->setText(windDirectionText(info.getWindDirection()));
    cloudinessValue->setText(QString("%1%").arg(info.getCloudiness()));
}

void WeatherCard::setCityName(const QString &cityName)
{
    cityLabel->setText(cityName);
}

void WeatherCard::clearData()
{
    setDefaultValues();
}

QLabel *WeatherCard::createValueLabel()
{
    auto *label = new QLabel(this);
    label->setObjectName("metricValue");
    return label;
}

void WeatherCard::setupLayout()
{
    auto *cardLayout = new QVBoxLayout(this);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(10);

    cardLayout->addWidget(cityLabel);

    auto *headerLayout = new QHBoxLayout;
    headerLayout->addStretch(1);
    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(temperatureLabel);
    headerLayout->addStretch(1);
    cardLayout->addLayout(headerLayout);

    descriptionLabel->setAlignment(Qt::AlignCenter);
    feelsLikeLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(descriptionLabel);
    cardLayout->addWidget(feelsLikeLabel);
    cardLayout->addWidget(timeLabel);
    cardLayout->addSpacing(8);

    auto *metricsLayout = new QGridLayout;
    metricsLayout->setHorizontalSpacing(48);
    metricsLayout->setVerticalSpacing(8);
    // Two title/value columns per row; the layout takes ownership of the labels
    const auto addMetric = [this, metricsLayout](int row, int column, const QString &title,
                                                 QLabel *value) {
        auto *titleLabel = new QLabel(title, this);
        titleLabel->setObjectName("metricTitle");
        metricsLayout->addWidget(titleLabel, row, column * 2);
        metricsLayout->addWidget(value, row, column * 2 + 1);
    };
    addMetric(0, 0, "Влажность", humidityValue);
    addMetric(0, 1, "Давление", pressureValue);
    addMetric(1, 0, "Ветер", windSpeedValue);
    addMetric(1, 1, "Направление", windDirectionValue);
    addMetric(2, 0, "Облачность", cloudinessValue);
    cardLayout->addLayout(metricsLayout);
}

void WeatherCard::setDefaultValues()
{
    temperatureLabel->setText("-- °C");
    feelsLikeLabel->setText("Ощущается как -- °C");
    descriptionLabel->clear();
    timeLabel->clear();
    iconLabel->clear();
    humidityValue->setText("--%");
    pressureValue->setText("-- hPa");
    windSpeedValue->setText("-- м/с");
    windDirectionValue->setText("--");
    cloudinessValue->setText("--%");
}

void WeatherCard::applyStyles()
{
    setStyleSheet(R"(
        QWidget#card {
            background-color: rgba(255, 255, 255, 16);
            border: 1px solid rgba(255, 255, 255, 24);
            border-radius: 18px;
        }
        QLabel {
            color: #e8eef2;
            background: transparent;
        }
        QLabel#city { font-size: 24px; font-weight: bold; }
        QLabel#temperature { font-size: 52px; font-weight: bold; }
        QLabel#iconEmoji { font-size: 56px; background: transparent; }
        QLabel#secondary { color: #9fb3c8; font-size: 15px; }
        QLabel#metricTitle { color: #9fb3c8; font-size: 13px; }
        QLabel#metricValue { font-size: 15px; font-weight: bold; }
    )");
}

QString WeatherCard::iconEmoji(const QString &iconCode)
{
    // OpenWeatherMap icon codes -> universally readable emoji
    // https://openweathermap.org/weather-conditions
    static const QHash<QString, QString> emojiByCode = {
        {"01d", "☀️"}, {"01n", "🌙"},
        {"02d", "🌤"}, {"02n", "☁️"},
        {"03d", "⛅"}, {"03n", "☁️"},
        {"04d", "☁️"}, {"04n", "☁️"},
        {"09d", "🌧"}, {"09n", "🌧"},
        {"10d", "🌦"}, {"10n", "🌧"},
        {"11d", "⛈"}, {"11n", "⛈"},
        {"13d", "❄️"}, {"13n", "❄️"},
        {"50d", "🌫"}, {"50n", "🌫"},
    };
    return emojiByCode.value(iconCode, "🌡");
}

QString WeatherCard::formatTemperature(const double temperature)
{
    return QString("%1%2 °C").arg(temperature > 0 ? "+" : "").arg(temperature, 0, 'f', 1);
}

QString WeatherCard::windDirectionText(const int degrees)
{
    static const QStringList directions = {"С", "СВ", "В", "ЮВ", "Ю", "ЮЗ", "З", "СЗ"};
    const int index = static_cast<int>((degrees + 22.5) / 45.0) % 8;
    return QString("%1 (%2°)").arg(directions.at(index)).arg(degrees);
}
