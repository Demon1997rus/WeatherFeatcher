#pragma once

#include <QObject>

#include "services/httpservice.h"
#include "services/weatherconverter.h"
#include "widgets/weatherwidget.h"

class WeatherController : public QObject {
    Q_OBJECT
public:
    explicit WeatherController(QObject *parent = nullptr);

private:
    HttpService httpService;
    WeatherConverter converter;
    WeatherWidget weatherWidget;
};
