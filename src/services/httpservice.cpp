#include "httpservice.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "utils/config.h"

namespace {
constexpr int requestTimeoutMs = 10000;
}

HttpService::HttpService(QObject *parent) :
    QObject(parent), networkManager(new QNetworkAccessManager(this))
{
}

void HttpService::fetchWeatherData(const double latitude, const double longitude)
{
    QUrl url("https://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;
    query.addQueryItem("lat", QString::number(latitude, 'f', 6));
    query.addQueryItem("lon", QString::number(longitude, 'f', 6));
    query.addQueryItem("appid", Config::instance().apiKey());
    query.addQueryItem("lang", "ru"); // Weather descriptions in Russian
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setTransferTimeout(requestTimeoutMs);

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit requestError(QString("Запрос не выполнен: %1").arg(reply->errorString()));
            return;
        }

        const QByteArray response = reply->readAll();
        const int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != 200) {
            emit requestError(QString("Ошибка запроса с кодом ответа: %1\nОтвет: %2")
                                  .arg(statusCode)
                                  .arg(QString::fromUtf8(response)));
            return;
        }

        emit weatherDataReceived(response);
    });
}
