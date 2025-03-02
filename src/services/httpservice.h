#pragma once

#include <QObject>

class HttpService : public QObject {
    Q_OBJECT
public:
    explicit HttpService(QObject *parent = nullptr);

public slots:
    void fetchWeatherData(const double latitude, const double longitude) const;

signals:
    void weatherDataReceived(const QByteArray &response) const;
    void requestError(const QString &errorMessage) const;
};
