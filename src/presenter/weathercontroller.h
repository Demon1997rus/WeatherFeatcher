#pragma once

#include <QObject>

#include "services/httpservice.h"
#include "services/weatherconverter.h"
#include "widgets/weatherwidget.h"

class CityDirectory;

class WeatherController : public QObject {
    Q_OBJECT
public:
    explicit WeatherController(const CityDirectory &cities, QObject *parent = nullptr);

private:
    HttpService httpService;
    WeatherConverter converter;
    WeatherWidget weatherWidget;
};
