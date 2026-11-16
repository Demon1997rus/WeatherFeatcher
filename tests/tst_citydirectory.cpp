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
    void findPrefersMostPopulatedOnDuplicateName();
    void searchTreatsYoAsYe();
    void entryAtResolvesDuplicateNamesExactly();

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
        {"n":"Пекин","a":"Beijing","c":"CN","la":39.9,"lo":116.4,"p":18900000},
        {"n":"Хэфэй","a":"Hefei","c":"CN","la":31.86,"lo":117.28,"p":9400000},
        {"n":"Хэфэй","a":"Luzhou","c":"CN","la":28.89,"lo":105.43,"p":990000},
        {"n":"Кёльн","a":"Cologne","c":"DE","la":50.93,"lo":6.95,"p":1000000}
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

void TestCityDirectory::findPrefersMostPopulatedOnDuplicateName()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    // Two entries share "Хэфэй, CN": the most populated one must win
    CityDirectory::Entry entry;
    QVERIFY(directory.find("Хэфэй, CN", entry));
    QCOMPARE(entry.coordinates, QPointF(31.86, 117.28));
}

void TestCityDirectory::searchTreatsYoAsYe()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    // Users often type "е" instead of "ё" (and vice versa)
    const auto results = directory.search("кельн", 10);
    QCOMPARE(results.size(), size_t(1));
    QCOMPARE(results.front().country, QString("DE"));
    QCOMPARE(directory.search("Кёльн", 10).size(), size_t(1));

    CityDirectory::Entry entry;
    QVERIFY(directory.bestMatch("Кельн", entry));
    QCOMPARE(entry.country, QString("DE"));
    QVERIFY(directory.find("Кельн, DE", entry));
    QCOMPARE(entry.country, QString("DE"));
}

void TestCityDirectory::entryAtResolvesDuplicateNamesExactly()
{
    CityDirectory directory;
    QVERIFY(directory.load(fixturePath));

    // Both "Хэфэй, CN" rows of the popup must lead to their own coordinates
    const auto results = directory.search("хэф", 10);
    QCOMPARE(results.size(), size_t(2));
    QVERIFY(results[0].id != results[1].id);

    CityDirectory::Entry entry;
    QVERIFY(directory.entryAt(results[1].id, entry));
    QCOMPARE(entry.coordinates, QPointF(28.89, 105.43));
    QVERIFY(directory.entryAt(results[0].id, entry));
    QCOMPARE(entry.coordinates, QPointF(31.86, 117.28));

    QVERIFY(!directory.entryAt(-1, entry));
    QVERIFY(!directory.entryAt(1000, entry));
}

QTEST_GUILESS_MAIN(TestCityDirectory)

#include "tst_citydirectory.moc"
