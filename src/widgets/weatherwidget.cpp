#include "weatherwidget.h"

#include <QAction>
#include <QDateTime>
#include <QGridLayout>
#include <QHash>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QListWidget>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

#include "utils/citydirectory.h"

namespace {
constexpr int refreshIntervalMs = 10 * 60 * 1000; // Refresh weather every 10 minutes
constexpr int searchDebounceMs = 200;             // Search after typing pauses
constexpr int maxSearchResults = 10;
}

WeatherWidget::WeatherWidget(const CityDirectory &cities, QWidget *parent) :
    QWidget(parent), cities(cities)
{
    setWindowTitle("Weather Fetcher");

    initWidgets();
    setupLayout();
    initConnects();
    setDefaultValues();
    applyStyles();

    // Prefer Moscow as the start city; fall back to the most populated one
    CityDirectory::Entry entry;
    if (cities.bestMatch("Москва", entry) || cities.bestMatch("", entry)) {
        selectCity(entry);
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

void WeatherWidget::displayError(const QString &errorMessage)
{
    setDefaultValues();
    // Non-modal error reporting: a status line keeps the UI usable
    statusLabel->setText(errorMessage);
}

void WeatherWidget::onCityActivated(const QString &displayName)
{
    CityDirectory::Entry entry;
    if (cities.find(displayName, entry)) {
        selectCity(entry);
    }
}

void WeatherWidget::onSearchAccepted()
{
    // Enter with an open popup activates the highlighted row
    if (searchPopup->isVisible() && searchPopup->currentRow() >= 0) {
        onCityActivated(searchPopup->currentItem()->text());
        return;
    }

    const QString text = searchField->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    CityDirectory::Entry entry;
    if (cities.find(text, entry) || cities.bestMatch(text, entry)) {
        selectCity(entry);
    } else {
        statusLabel->setText(QString("Город \"%1\" не найден в базе.").arg(text));
    }
}

void WeatherWidget::performSearch()
{
    const QString text = searchField->text().trimmed();
    if (text.isEmpty()) {
        showBrowseList();
        return;
    }

    const auto results = cities.search(text, maxSearchResults);
    if (results.empty()) {
        hideSearchPopup();
        statusLabel->setText(QString("Город \"%1\" не найден в базе.").arg(text));
        return;
    }

    showResults(results);
}

void WeatherWidget::showBrowseList()
{
    // Browse mode: top cities by population, regardless of the field text
    showResults(cities.search("", maxSearchResults));
}

void WeatherWidget::showResults(const std::vector<CityDirectory::Entry> &results)
{
    statusLabel->clear();
    searchPopup->clear();
    for (const auto &entry : results) {
        searchPopup->addItem(CityDirectory::displayName(entry));
    }

    // Popup right under the search field, same width
    const QPoint fieldPos = searchField->mapTo(this, QPoint(0, searchField->height() + 4));
    const int rowHeight = 32;
    searchPopup->setGeometry(fieldPos.x(), fieldPos.y(), searchField->width(),
                             rowHeight * static_cast<int>(results.size()) + 8);
    searchPopup->setCurrentRow(0);
    searchPopup->show();
    searchPopup->raise();
}

void WeatherWidget::hideSearchPopup()
{
    searchPopup->hide();
}

bool WeatherWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == searchField && event->type() == QEvent::FocusIn) {
        // Focusing an empty field shows the top cities right away
        if (searchField->text().trimmed().isEmpty()) {
            QTimer::singleShot(0, this, &WeatherWidget::showBrowseList);
        }
        return false;
    }

    if (watched == searchField && event->type() == QEvent::FocusOut) {
        // Delayed hide so a click on a popup row is processed first
        QTimer::singleShot(150, this, &WeatherWidget::hideSearchPopup);
        return false;
    }

    if (watched == searchField && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            hideSearchPopup();
            return true;
        }
        if ((keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up)) {
            if (!searchPopup->isVisible()) {
                showBrowseList();
                return true;
            }
            const int step = (keyEvent->key() == Qt::Key_Down) ? 1 : -1;
            const int next = qBound(0, searchPopup->currentRow() + step,
                                    searchPopup->count() - 1);
            searchPopup->setCurrentRow(next);
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void WeatherWidget::selectCity(const CityDirectory::Entry &entry)
{
    hasSelection = true;
    currentCoordinates = entry.coordinates;
    cityLabel->setText(entry.name);

    // Setting the text programmatically must not reopen the popup
    suppressSearch = true;
    searchField->setText(CityDirectory::displayName(entry));
    suppressSearch = false;
    hideSearchPopup();

    emit requestWeatherData(entry.coordinates.x(), entry.coordinates.y());
}

QLabel *WeatherWidget::createValueLabel()
{
    auto *label = new QLabel(this);
    label->setObjectName("metricValue");
    return label;
}

void WeatherWidget::initWidgets()
{
    searchField = new QLineEdit(this);
    searchField->setObjectName("search");
    searchField->setPlaceholderText("Начните вводить город...");
    searchField->setClearButtonEnabled(true);
    searchField->installEventFilter(this);

    // Drop-down affordance: opens the top-cities list on click
    auto *dropdownAction =
        searchField->addAction(style()->standardIcon(QStyle::SP_ArrowDown),
                               QLineEdit::TrailingPosition);
    connect(dropdownAction, &QAction::triggered, this, [this]() {
        if (searchPopup->isVisible()) {
            hideSearchPopup();
        } else {
            searchField->setFocus();
            showBrowseList();
        }
    });

    searchPopup = new QListWidget(this);
    searchPopup->setObjectName("searchPopup");
    searchPopup->setMouseTracking(true);
    searchPopup->hide();

    searchDebounce = new QTimer(this);
    searchDebounce->setSingleShot(true);
    searchDebounce->setInterval(searchDebounceMs);

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

    statusLabel = new QLabel(this);
    statusLabel->setObjectName("status");

    refreshTimer = new QTimer(this);
}

void WeatherWidget::setupLayout()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(48, 24, 48, 24);

    searchField->setFixedWidth(420);
    rootLayout->addWidget(searchField, 0, Qt::AlignHCenter);
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

    rootLayout->addWidget(card, 0, Qt::AlignHCenter);
    rootLayout->addStretch(1);

    statusLabel->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(statusLabel);
}

void WeatherWidget::initConnects()
{
    connect(searchField, &QLineEdit::textChanged, this, [this](const QString &) {
        if (!suppressSearch) {
            searchDebounce->start();
        }
    });
    connect(searchDebounce, &QTimer::timeout, this, &WeatherWidget::performSearch);
    connect(searchPopup, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *item) { onCityActivated(item->text()); });
    connect(searchField, &QLineEdit::returnPressed, this, &WeatherWidget::onSearchAccepted);

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
        QLabel#iconEmoji { font-size: 56px; background: transparent; }
        QLabel#secondary { color: #9fb3c8; font-size: 15px; }
        QLabel#metricTitle { color: #9fb3c8; font-size: 13px; }
        QLabel#metricValue { font-size: 15px; font-weight: bold; }
        QLabel#status { color: #ff7b72; }
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

    // Search popup styling
    searchPopup->setStyleSheet(R"(
        QListWidget#searchPopup {
            background-color: #20303c;
            color: #e8eef2;
            border: 1px solid rgba(255, 255, 255, 40);
            border-radius: 8px;
            padding: 4px;
            outline: none;
            font-size: 14px;
        }
        QListWidget#searchPopup::item {
            padding: 7px 10px;
            border-radius: 5px;
        }
        QListWidget#searchPopup::item:selected {
            background-color: #2c5364;
        }
        QListWidget#searchPopup::item:hover {
            background-color: rgba(44, 83, 100, 120);
        }
    )");
}

QString WeatherWidget::iconEmoji(const QString &iconCode)
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
