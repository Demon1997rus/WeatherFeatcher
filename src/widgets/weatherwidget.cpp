#include "weatherwidget.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

#include "citysearchbox.h"
#include "weathercard.h"

namespace {
constexpr int refreshIntervalMs = 10 * 60 * 1000; // Refresh weather every 10 minutes
}

WeatherWidget::WeatherWidget(const CityDirectory &cities, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Weather Fetcher");

    searchBox = new CitySearchBox(cities, this);
    card = new WeatherCard(this);
    statusLabel = new QLabel(this);
    statusLabel->setObjectName("status");
    statusLabel->setAlignment(Qt::AlignCenter);
    refreshTimer = new QTimer(this);

    setupLayout();
    initConnects();
    applyStyles();

    // Prefer Moscow as the start city; fall back to the most populated one
    CityDirectory::Entry entry;
    if (cities.bestMatch("Москва", entry) || cities.bestMatch("", entry)) {
        searchBox->setCityDisplay(entry);
        onCitySelected(entry);
    }
}

void WeatherWidget::loadCurrentCityWeather()
{
    if (hasSelection) {
        emit requestWeatherData(currentCoordinates.x(), currentCoordinates.y());
    }
}

void WeatherWidget::displayWeatherData(const WeatherInfo &info)
{
    statusLabel->clear();
    card->displayWeatherData(info);
}

void WeatherWidget::displayError(const QString &errorMessage)
{
    card->clearData();
    // Non-modal error reporting: a status line keeps the UI usable
    statusLabel->setText(errorMessage);
}

void WeatherWidget::onCitySelected(const CityDirectory::Entry &entry)
{
    hasSelection = true;
    currentCoordinates = entry.coordinates;
    card->setCityName(entry.name);
    emit requestWeatherData(entry.coordinates.x(), entry.coordinates.y());
}

void WeatherWidget::setupLayout()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(48, 24, 48, 24);

    searchBox->setFixedWidth(420);
    rootLayout->addWidget(searchBox, 0, Qt::AlignHCenter);
    rootLayout->addStretch(1);

    rootLayout->addWidget(card, 0, Qt::AlignHCenter);
    rootLayout->addStretch(1);

    rootLayout->addWidget(statusLabel);
}

void WeatherWidget::initConnects()
{
    connect(searchBox, &CitySearchBox::citySelected, this, &WeatherWidget::onCitySelected);
    connect(searchBox, &CitySearchBox::searchFailed, this, [this](const QString &text) {
        statusLabel->setText(QString("Город \"%1\" не найден в базе.").arg(text));
    });

    // Periodic refresh of the currently selected city
    connect(refreshTimer, &QTimer::timeout, this, &WeatherWidget::loadCurrentCityWeather);
    refreshTimer->start(refreshIntervalMs);
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
        QLabel#status { color: #ff7b72; background: transparent; }
        QLineEdit#search {
            background-color: rgba(255, 255, 255, 16);
            color: #e8eef2;
            border: 1px solid rgba(255, 255, 255, 40);
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
        }
        QLineEdit#search:focus {
            border: 1px solid rgba(120, 190, 255, 140);
        }
    )");
}
