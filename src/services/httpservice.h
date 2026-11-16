#pragma once

#include <QObject>
#include <QPointer>

class QNetworkAccessManager;
class QNetworkReply;

// Fetches current weather from OpenWeatherMap. Only the latest request is
// kept alive: a new fetch aborts the previous one, so a slow reply for a city
// the user has already left can never overwrite the current one.
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
    void onReplyFinished(QNetworkReply *reply);

    QNetworkAccessManager *networkManager = nullptr;
    QPointer<QNetworkReply> currentReply;
};
