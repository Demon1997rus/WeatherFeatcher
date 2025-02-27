#pragma once

#include <QObject>
#include <QString>

#include <string>
#include <vector>

#include "structures/citycoordinate.h"

class Config : public QObject {
    Q_OBJECT
public:
    static Config &instance();
    bool loadConfig(const QString &configPath);

private:
    Q_DISABLE_COPY(Config);
    explicit Config(QObject *parent = nullptr);

private:
    std::string api_key;
    std::vector<CityCoordinate> coordinates_cities;
};
