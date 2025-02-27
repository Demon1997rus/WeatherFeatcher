#pragma once

#include <QObject>

class WeatherController : public QObject {
    Q_OBJECT
public:
    explicit WeatherController(QObject *parent = nullptr);

signals:
};
