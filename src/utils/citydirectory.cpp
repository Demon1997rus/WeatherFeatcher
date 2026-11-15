#include "citydirectory.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool CityDirectory::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        return false;
    }

    const QJsonArray cities = doc.array();
    entries_.reserve(cities.size());

    QStringList aliases;
    int index = 0;
    for (const auto &value : cities) {
        const QJsonObject obj = value.toObject();

        Entry entry;
        entry.name = obj["n"].toString();
        entry.alias = obj["a"].toString();
        entry.country = obj["c"].toString();
        entry.coordinates = QPointF(obj["la"].toDouble(), obj["lo"].toDouble());

        const QString display = displayName(entry);
        displayNames_.append(display);
        indexByName_.insert(display, index);
        if (!entry.alias.isEmpty() && entry.alias != entry.name) {
            const QString aliasDisplay = entry.alias + ", " + entry.country;
            indexByName_.insert(aliasDisplay, index);
            // Aliases rank after all display names in the completion popup
            aliases.append(aliasDisplay);
        }

        entries_.push_back(std::move(entry));
        ++index;
    }
    displayNames_.append(aliases);

    return !entries_.empty();
}

const QStringList &CityDirectory::displayNames() const
{
    return displayNames_;
}

bool CityDirectory::find(const QString &displayName, Entry &out) const
{
    const auto it = indexByName_.constFind(displayName);
    if (it == indexByName_.constEnd()) {
        return false;
    }
    out = entries_.at(it.value());
    return true;
}

bool CityDirectory::bestMatch(const QString &text, Entry &out) const
{
    for (const Entry &entry : entries_) {
        if (entry.name.contains(text, Qt::CaseInsensitive) ||
            entry.alias.contains(text, Qt::CaseInsensitive)) {
            out = entry;
            return true;
        }
    }
    return false;
}

QString CityDirectory::displayName(const Entry &entry)
{
    return entry.name + ", " + entry.country;
}
