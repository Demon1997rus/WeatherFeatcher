#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "presenter/weathercontroller.h"
#include "utils/config.h"
#include "widgets/weatherwidget.h"

namespace {

// Hidden developer option: renders the widget with sample data into a PNG and quits.
// Usage: weather-fetcher --screenshot out.png [-platform offscreen]
int runScreenshotMode(QApplication &app, const QString &outputPath)
{
    WeatherWidget widget;

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
    sample.setCityName("Москва");

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

    const QStringList args = app.arguments();
    const int screenshotIndex = args.indexOf("--screenshot");
    if (screenshotIndex != -1 && screenshotIndex + 1 < args.size()) {
        return runScreenshotMode(app, args.at(screenshotIndex + 1));
    }

    // Global config is loaded from the "data" directory next to the executable
    const QString configPath = QCoreApplication::applicationDirPath() + "/data/config.json";
    if (!Config::instance().loadConfig(configPath)) {
        qFatal("Couldn't load the config: \"%s\"", configPath.toUtf8().constData());
    }

    WeatherController controller(&app);

    return app.exec();
}
