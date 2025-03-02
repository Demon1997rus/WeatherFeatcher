#pragma once

#include <QObject>

#include "structures/weatherinfo.h"

class WeatherConverter : public QObject {
    Q_OBJECT
public:
    explicit WeatherConverter(QObject *parent = nullptr);

public slots:
    void convertToWeatherInfo(const QByteArray &data) const;

signals:
    void weatherInfoReady(const WeatherInfo &info) const;
};
