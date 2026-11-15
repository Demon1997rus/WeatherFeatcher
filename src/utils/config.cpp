#include "config.h"

Config &Config::instance()
{
    static Config instance;
    return instance;
}

Config::Config() : apiKey_(qgetenv("OPENWEATHER_API_KEY"))
{
}

const QString &Config::apiKey() const
{
    return this->apiKey_;
}
