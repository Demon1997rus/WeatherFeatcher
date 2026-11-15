#include "weatherwidget.h"

#include <QCoreApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointF>
#include <QTimer>
#include <QVBoxLayout>

#include "utils/config.h"

namespace {
constexpr int refreshIntervalMs = 10 * 60 * 1000; // Refresh weather every 10 minutes
}

WeatherWidget::WeatherWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Weather Fetcher");

    initWidgets();
    setupLayout();
    initConnects();
    setDefaultValues();
    applyStyles();
}

void WeatherWidget::loadCurrentCityWeather()
{
    onCitySelected(cbCities->currentIndex());
}

void WeatherWidget::displayWeatherData(const WeatherInfo &info)
{
    statusLabel->clear();
    updateWeatherIcon(info.getIconCode());

    if (!info.getCityName().isEmpty()) {
        cityLabel->setText(info.getCityName());
    }
    temperatureLabel->setText(formatTemperature(info.getTemperature()));
    feelsLikeLabel->setText(QString("Ощущается как %1").arg(formatTemperature(info.getFeelsLike())));
    descriptionLabel->setText(info.getWeatherDescription());
    humidityValue->setText(QString("%1%").arg(info.getHumidity()));
    pressureValue->setText(QString("%1 hPa").arg(info.getPressure()));
    windSpeedValue->setText(QString("%1 м/с").arg(info.getWindSpeed()));
    windDirectionValue->setText(windDirectionText(info.getWindDirection()));
    cloudinessValue->setText(QString("%1%").arg(info.getCloudiness()));
}

void WeatherWidget::displayError(const QString &errorMessage)
{
    setDefaultValues();
    // Non-modal error reporting: a status line keeps the UI usable
    statusLabel->setText(errorMessage);
}

void WeatherWidget::onCitySelected(int index)
{
    if (index < 0) {
        // Editable combo: typed text does not match any known city
        statusLabel->setText("Город не найден в списке. Выберите его стрелкой справа.");
        return;
    }
    const QPointF coordinates = cbCities->itemData(index).toPointF();
    cityLabel->setText(cbCities->itemText(index));
    emit requestWeatherData(coordinates.x(), coordinates.y());
}

QLabel *WeatherWidget::createValueLabel()
{
    auto *label = new QLabel(this);
    label->setObjectName("metricValue");
    return label;
}

void WeatherWidget::initWidgets()
{
    cbCities = new QComboBox(this);
    searchField = new QLineEdit(this);
    searchField->setPlaceholderText("Поиск города...");
    cbCities->setEditable(true);
    cbCities->setLineEdit(searchField);

    // Cities from the config: name is displayed, coordinates travel in item data
    for (const auto &city : Config::instance().coordinatesCities()) {
        cbCities->addItem(QString::fromStdString(city.getCity()),
                          QPointF(city.getLatitude(), city.getLongitude()));
    }

    iconLabel = new QLabel(this);
    cityLabel = new QLabel(this);
    cityLabel->setObjectName("city");
    cityLabel->setAlignment(Qt::AlignCenter);
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

    statusLabel = new QLabel(this);
    statusLabel->setObjectName("status");

    refreshTimer = new QTimer(this);
    iconDownloader = new QNetworkAccessManager(this);
}

void WeatherWidget::setupLayout()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(48, 24, 48, 24);

    cbCities->setFixedWidth(420);
    rootLayout->addWidget(cbCities, 0, Qt::AlignHCenter);
    rootLayout->addStretch(1);

    // Central card with all weather data
    auto *card = new QWidget(this);
    card->setObjectName("card");
    card->setFixedWidth(520);
    auto *cardLayout = new QVBoxLayout(card);
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

    rootLayout->addWidget(card, 0, Qt::AlignHCenter);
    rootLayout->addStretch(1);

    statusLabel->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(statusLabel);
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
    temperatureLabel->setText("-- °C");
    feelsLikeLabel->setText("Ощущается как -- °C");
    descriptionLabel->clear();
    iconLabel->clear();
    humidityValue->setText("--%");
    pressureValue->setText("-- hPa");
    windSpeedValue->setText("-- м/с");
    windDirectionValue->setText("--");
    cloudinessValue->setText("--%");
}

void WeatherWidget::applyStyles()
{
    // Resolution-independent diagonal gradient as the window background
    QLinearGradient gradient(0.0, 0.0, 1.0, 1.0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.0, QColor("#0f2027"));
    gradient.setColorAt(0.5, QColor("#203a43"));
    gradient.setColorAt(1.0, QColor("#2c5364"));
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(gradient));
    setAutoFillBackground(true);
    setPalette(palette);

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
        QLabel#secondary { color: #9fb3c8; font-size: 15px; }
        QLabel#metricTitle { color: #9fb3c8; font-size: 13px; }
        QLabel#metricValue { font-size: 15px; font-weight: bold; }
        QLabel#status { color: #ff7b72; }
        QComboBox {
            background-color: rgba(255, 255, 255, 16);
            color: #e8eef2;
            border: 1px solid rgba(255, 255, 255, 40);
            border-radius: 8px;
            padding: 6px 12px;
            font-size: 14px;
        }
        QComboBox:hover, QComboBox:focus {
            border: 1px solid rgba(120, 190, 255, 140);
        }
        QComboBox::drop-down { border: none; width: 30px; }
        QComboBox::down-arrow {
            width: 0;
            height: 0;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #9fb3c8;
        }
        QComboBox QAbstractItemView {
            background-color: #20303c;
            color: #e8eef2;
            selection-background-color: #2c5364;
            border: 1px solid rgba(255, 255, 255, 40);
        }
        QLineEdit { background: transparent; color: #e8eef2; border: none; }
    )");
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

QString WeatherWidget::formatTemperature(const double temperature)
{
    return QString("%1%2 °C").arg(temperature > 0 ? "+" : "").arg(temperature, 0, 'f', 1);
}

QString WeatherWidget::windDirectionText(const int degrees)
{
    static const QStringList directions = {"С", "СВ", "В", "ЮВ", "Ю", "ЮЗ", "З", "СЗ"};
    const int index = static_cast<int>((degrees + 22.5) / 45.0) % 8;
    return QString("%1 (%2°)").arg(directions.at(index)).arg(degrees);
}
