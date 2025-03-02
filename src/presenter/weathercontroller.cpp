#include "weathercontroller.h"

WeatherController::WeatherController(QObject *parent) : QObject(parent)
{
    connect(&httpService, &HttpService::weatherDataReceived, &converter,
            &WeatherConverter::convertToWeatherInfo);

    connect(&converter, &WeatherConverter::weatherInfoReady, &weatherWidget,
            &WeatherWidget::displayWeatherData);

    connect(&weatherWidget, &WeatherWidget::requestWeatherData, &httpService,
            &HttpService::fetchWeatherData);

    connect(&httpService, &HttpService::requestError, &weatherWidget,
            &WeatherWidget::displayError);

    weatherWidget.loadCurrentCityWeather();

    weatherWidget.showMaximized();
}
