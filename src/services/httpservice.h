#pragma once

#include <QObject>

class QNetworkAccessManager;

class HttpService : public QObject {
    Q_OBJECT
public:
    explicit HttpService(QObject *parent = nullptr);

public slots:
    void fetchWeatherData(double latitude, double longitude);

signals:
    void weatherDataReceived(const QByteArray &response);
    void requestError(const QString &errorMessage);

private:
    QNetworkAccessManager *networkManager = nullptr;
};
