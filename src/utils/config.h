#pragma once

#include <QObject>
#include <QString>

#include <string>
#include <vector>

#include "structures/citycoordinate.h"

class Config {
public:
    static Config &instance();
    bool loadConfig(const QString &configPath);

public:
    const std::string &apiKey() const;
    const std::vector<CityCoordinate> &coordinatesCities() const;

private:
    Q_DISABLE_COPY(Config);
    Config() = default;

private:
    std::string api_key;
    std::vector<CityCoordinate> coordinates_cities;
};
