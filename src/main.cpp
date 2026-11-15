#include <QApplication>
#include <QDebug>

#include "presenter/weathercontroller.h"
#include "utils/config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Global config is loaded from the "data" directory next to the executable
    const QString configPath = QCoreApplication::applicationDirPath() + "/data/config.json";
    if (!Config::instance().loadConfig(configPath)) {
        qFatal("Couldn't load the config: \"%s\"", configPath.toUtf8().constData());
    }

    WeatherController controller(&app);

    return app.exec();
}
