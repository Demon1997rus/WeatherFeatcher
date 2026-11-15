#include <QtTest>

#include "utils/citydirectory.h"

class TestCityDirectory : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void findsByDisplayName();
    void findsByAsciiAlias();
    void bestMatchPrefersMostPopulated();
    void bestMatchSearchesAliases();
    void searchRanksPrefixOverSubstring();
    void returnsFalseOnUnknown();

private:
    QString fixturePath;
};

void TestCityDirectory::initTestCase()
{
    fixturePath = QDir::temp().filePath("cities_fixture.json");
    QFile fixture(fixturePath);
    QVERIFY(fixture.open(QIODevice::WriteOnly));
    fixture.write(R"([
        {"n":"Москва","a":"Moscow","c":"RU","la":55.75,"lo":37.62,"p":12500000},
        {"n":"Самара","a":"Samara","c":"RU","la":53.2,"lo":50.15,"p":1100000},
        {"n":"Самара","a":"Samara","c":"US","la":40.0,"lo":-100.0,"p":20000},
        {"n":"Пекин","a":"Beijing","c":"CN","la":39.9,"lo":116.4,"p":18900000}
    ])");
    fixture.close();
}

void TestCityDirectory::findsByDisplayName()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    CityDirectory::Entry entry;
    QVERIFY(directory.find("Москва, RU", entry));
    QCOMPARE(entry.name, QString("Москва"));
    QCOMPARE(entry.coordinates, QPointF(55.75, 37.62));
}

void TestCityDirectory::findsByAsciiAlias()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    CityDirectory::Entry entry;
    QVERIFY(directory.find("Beijing, CN", entry));
    QCOMPARE(entry.name, QString("Пекин"));
}

void TestCityDirectory::bestMatchPrefersMostPopulated()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    // "Самара" exists twice: RU (1.1M) must win over US (20k)
    CityDirectory::Entry entry;
    QVERIFY(directory.bestMatch("самара", entry));
    QCOMPARE(entry.country, QString("RU"));
}

void TestCityDirectory::bestMatchSearchesAliases()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    CityDirectory::Entry entry;
    QVERIFY(directory.bestMatch("beij", entry));
    QCOMPARE(entry.name, QString("Пекин"));
}

void TestCityDirectory::returnsFalseOnUnknown()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    CityDirectory::Entry entry;
    QVERIFY(!directory.find("Несуществующий, RU", entry));
    QVERIFY(!directory.bestMatch("qwertyuiop", entry));
}

void TestCityDirectory::searchRanksPrefixOverSubstring()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    // "сам" is a prefix of both Самара entries; RU (1.1M) must beat US (20k)
    const auto results = directory.search("сам", 10);
    QCOMPARE(results.size(), size_t(2));
    QCOMPARE(results.front().country, QString("RU"));
}

QTEST_GUILESS_MAIN(TestCityDirectory)

#include "tst_citydirectory.moc"
