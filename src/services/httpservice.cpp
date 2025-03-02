#include "httpservice.h"

#include <memory>
#include <sstream>

#include <curl/curl.h>

#include "utils/config.h"

HttpService::HttpService(QObject *parent) : QObject(parent) {}

size_t writeCallback(void *contents, size_t size, size_t nmemb, QByteArray *userp)
{
    const size_t res = size * nmemb;
    userp->reserve(userp->size() + static_cast<int>(res));
    userp->append(static_cast<const char *>(contents), static_cast<int>(res));
    return res;
}

void curlDeleter(CURL *curl)
{
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

void HttpService::fetchWeatherData(const double latitude, const double longitude) const
{
    std::unique_ptr<CURL, decltype(&curlDeleter)> curlHandle(curl_easy_init(), curlDeleter);
    if (!curlHandle) {
        emit requestError(
            "Не удалось инициализировать соединение. Пожалуйста, попробуйте еще раз или обратитесь в поддержку.");
        return;
    }

    std::ostringstream urlStream;
    urlStream << "https://api.openweathermap.org/data/2.5/weather?lat=" << latitude
              << "&lon=" << longitude << "&appid=" << Config::instance().apiKey();

    const std::string url = urlStream.str();

    QByteArray response;
    curl_easy_setopt(curlHandle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle.get(), CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle.get(), CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curlHandle.get());
    if (res != CURLE_OK) {
        const QString errorMessage =
            QString("Запрос не выполнен: %1").arg(curl_easy_strerror(res));
        emit requestError(errorMessage);
        return;
    }

    long responseCode;
    curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);

    if (responseCode != 200) {
        const QString errorMessage = QString("Ошибка запроса с кодом ответа: %1\nОтвет: %2")
                                         .arg(responseCode)
                                         .arg(QString(response));
        emit requestError(errorMessage);
        return;
    }

    emit weatherDataReceived(response);
}
