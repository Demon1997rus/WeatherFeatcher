#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "presenter/weathercontroller.h"
#include "utils/citydirectory.h"
#include "utils/config.h"
#include "widgets/weatherwidget.h"

namespace {

// Hidden developer option: renders the widget with sample data into a PNG and quits.
// Usage: weather-fetcher --screenshot out.png [-platform offscreen]
int runScreenshotMode(QApplication &app, const QString &outputPath, CityDirectory &cities)
{
    WeatherWidget widget(cities);

    WeatherInfo sample;
    sample.setTemperature(3.3);
    sample.setFeelsLike(0.4);
    sample.setWeatherDescription("Пасмурно");
    sample.setHumidity(74);
    sample.setPressure(1025);
    sample.setWindSpeed(3.03);
    sample.setWindDirection(231);
    sample.setCloudiness(96);
    sample.setIconCode("04d");
    sample.setTimezoneOffset(3 * 3600);

    widget.resize(1280, 800);
    widget.show();
    widget.displayWeatherData(sample);

    QTimer::singleShot(2000, &app, [&]() {
        widget.grab().save(outputPath);
        app.quit();
    });
    return app.exec();
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // The offline city database lives next to the executable
    CityDirectory cities;
    const QString citiesPath = QCoreApplication::applicationDirPath() + "/data/cities.json";
    if (!cities.load(citiesPath)) {
        qFatal("Couldn't load the city database: \"%s\"", citiesPath.toUtf8().constData());
    }

    // Screenshot mode needs no API key
    const QStringList args = app.arguments();
    const int screenshotIndex = args.indexOf("--screenshot");
    if (screenshotIndex != -1 && screenshotIndex + 1 < args.size()) {
        return runScreenshotMode(app, args.at(screenshotIndex + 1), cities);
    }

    if (Config::instance().apiKey().isEmpty()) {
        qFatal("Environment variable OPENWEATHER_API_KEY is not set");
    }

    WeatherController controller(cities, &app);

    return app.exec();
}
