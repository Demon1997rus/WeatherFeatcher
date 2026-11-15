#include <QtTest>

#include "services/weatherconverter.h"

Q_DECLARE_METATYPE(WeatherInfo)

class TestWeatherConverter : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void parsesValidJson();
    void rejectsInvalidJson();
    void defaultsOnMissingKeys();
};

void TestWeatherConverter::initTestCase()
{
    qRegisterMetaType<WeatherInfo>();
}

void TestWeatherConverter::parsesValidJson()
{
    const QByteArray json = R"({
        "main": {"temp": 276.42, "feels_like": 273.55, "humidity": 74, "pressure": 1025},
        "weather": [{"description": "overcast clouds", "icon": "04d"}],
        "wind": {"speed": 3.03, "deg": 231},
        "clouds": {"all": 96}
    })";

    WeatherConverter converter;
    QSignalSpy spy(&converter, &WeatherConverter::weatherInfoReady);

    converter.convertToWeatherInfo(json);

    QCOMPARE(spy.count(), 1);
    const WeatherInfo info = qvariant_cast<WeatherInfo>(spy.takeFirst().at(0));
    QCOMPARE(info.getTemperature(), 276.42 - 273.15);
    QCOMPARE(info.getFeelsLike(), 273.55 - 273.15);
    QCOMPARE(info.getHumidity(), 74);
    QCOMPARE(info.getPressure(), 1025);
    QCOMPARE(info.getWeatherDescription(), QString("overcast clouds"));
    QCOMPARE(info.getIconCode(), QString("04d"));
    QCOMPARE(info.getWindSpeed(), 3.03);
    QCOMPARE(info.getWindDirection(), 231);
    QCOMPARE(info.getCloudiness(), 96);
}

void TestWeatherConverter::rejectsInvalidJson()
{
    WeatherConverter converter;
    QSignalSpy spy(&converter, &WeatherConverter::weatherInfoReady);

    converter.convertToWeatherInfo("this is not json");

    QCOMPARE(spy.count(), 0);
}

void TestWeatherConverter::defaultsOnMissingKeys()
{
    // Missing sections must yield zero-initialized values, not garbage
    WeatherConverter converter;
    QSignalSpy spy(&converter, &WeatherConverter::weatherInfoReady);

    converter.convertToWeatherInfo("{}");

    QCOMPARE(spy.count(), 1);
    const WeatherInfo info = qvariant_cast<WeatherInfo>(spy.takeFirst().at(0));
    QCOMPARE(info.getTemperature(), 0.0);
    QCOMPARE(info.getHumidity(), 0);
    QCOMPARE(info.getWeatherDescription(), QString());
}

QTEST_GUILESS_MAIN(TestWeatherConverter)

#include "tst_weatherconverter.moc"
