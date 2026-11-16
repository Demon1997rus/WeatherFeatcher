#include "httpservice.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "utils/config.h"

namespace {
constexpr int requestTimeoutMs = 10000;

// User-facing text for a failed reply. Never built from errorString() or the
// request URL: both may contain the appid query item, i.e. the API key.
QString describeFailure(const int httpStatus, const QNetworkReply::NetworkError error,
                        const QByteArray &body)
{
    if (httpStatus >= 400) {
        switch (httpStatus) {
        case 401:
            return "Ошибка: сервер погоды отклонил API-ключ (401).";
        case 404:
            return "Ошибка: для этой точки нет данных о погоде (404).";
        case 429:
            return "Ошибка: превышен лимит запросов к серверу погоды (429), попробуйте позже.";
        default:
            break;
        }
        // OWM error bodies look like {"cod":500,"message":"..."}
        const QString message =
            QJsonDocument::fromJson(body).object().value("message").toString();
        return message.isEmpty()
                   ? QString("Ошибка сервера погоды (код %1).").arg(httpStatus)
                   : QString("Ошибка сервера погоды (код %1): %2").arg(httpStatus).arg(message);
    }

    switch (error) {
    case QNetworkReply::NoError: // Transport fine, but not the expected 200
        return QString("Ошибка: неожиданный ответ сервера погоды (код %1).").arg(httpStatus);
    case QNetworkReply::OperationCanceledError: // transferTimeout expired
    case QNetworkReply::TimeoutError:
        return QString("Ошибка: сервер погоды не ответил за %1 с.").arg(requestTimeoutMs / 1000);
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::NetworkSessionFailedError:
        return "Ошибка: нет соединения с сервером погоды.";
    case QNetworkReply::SslHandshakeFailedError:
        return "Ошибка: не удалось установить защищённое соединение с сервером погоды.";
    default:
        return QString("Ошибка сети (код %1).").arg(static_cast<int>(error));
    }
}
} // namespace

HttpService::HttpService(QObject *parent) :
    QObject(parent), networkManager(new QNetworkAccessManager(this))
{
}

void HttpService::fetchWeatherData(const double latitude, const double longitude)
{
    if (currentReply) {
        // Superseded: drop it silently (abort() would report a cancel error)
        currentReply->disconnect(this);
        currentReply->abort();
        currentReply->deleteLater();
    }

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
    currentReply = reply;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onReplyFinished(reply); });
}

void HttpService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply != currentReply) {
        return; // A newer request has replaced this one
    }
    currentReply = nullptr;

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray response = reply->readAll();

    // Qt flags HTTP 4xx/5xx as errors too, so the status is checked first
    if (reply->error() != QNetworkReply::NoError || statusCode != 200) {
        emit requestError(describeFailure(statusCode, reply->error(), response));
        return;
    }

    emit weatherDataReceived(response);
}
