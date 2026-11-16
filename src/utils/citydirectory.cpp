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
    searchKeys_.reserve(cities.size());

    // Entries come in population order, so the first entry registered under a
    // string is the most populated one; later duplicates must not replace it
    const auto registerString = [this](const QString &text, int index) {
        const QString key = foldForSearch(text);
        if (!indexByName_.contains(key)) {
            indexByName_.insert(key, index);
        }
    };

    QStringList aliases;
    int index = 0;
    for (const auto &value : cities) {
        const QJsonObject obj = value.toObject();

        Entry entry;
        entry.name = obj["n"].toString();
        entry.alias = obj["a"].toString();
        entry.country = obj["c"].toString();
        entry.coordinates = QPointF(obj["la"].toDouble(), obj["lo"].toDouble());
        entry.id = index;

        const QString display = displayName(entry);
        displayNames_.append(display);
        registerString(display, index);
        if (!entry.alias.isEmpty() && entry.alias != entry.name) {
            const QString aliasDisplay = entry.alias + ", " + entry.country;
            registerString(aliasDisplay, index);
            // Aliases rank after all display names in the completion popup
            aliases.append(aliasDisplay);
        }

        searchKeys_.push_back({foldForSearch(entry.name), foldForSearch(entry.alias)});
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
    const auto it = indexByName_.constFind(foldForSearch(displayName));
    if (it == indexByName_.constEnd()) {
        return false;
    }
    out = entries_.at(it.value());
    return true;
}

bool CityDirectory::entryAt(const int id, Entry &out) const
{
    if (id < 0 || id >= static_cast<int>(entries_.size())) {
        return false;
    }
    out = entries_.at(id);
    return true;
}

bool CityDirectory::bestMatch(const QString &text, Entry &out) const
{
    const QString needle = foldForSearch(text);
    for (size_t i = 0; i < entries_.size(); ++i) {
        const SearchKey &key = searchKeys_[i];
        if (key.name.contains(needle, Qt::CaseInsensitive) ||
            key.alias.contains(needle, Qt::CaseInsensitive)) {
            out = entries_[i];
            return true;
        }
    }
    return false;
}

std::vector<CityDirectory::Entry> CityDirectory::search(const QString &text, int limit) const
{
    const QString needle = foldForSearch(text);
    std::vector<Entry> prefixMatches;
    std::vector<Entry> substringMatches;
    prefixMatches.reserve(limit);
    substringMatches.reserve(limit);

    for (size_t i = 0; i < entries_.size(); ++i) {
        const SearchKey &key = searchKeys_[i];
        const bool namePrefix = key.name.startsWith(needle, Qt::CaseInsensitive);
        const bool aliasPrefix = key.alias.startsWith(needle, Qt::CaseInsensitive);
        if (namePrefix || aliasPrefix) {
            if (static_cast<int>(prefixMatches.size()) < limit) {
                prefixMatches.push_back(entries_[i]);
            }
        } else if (static_cast<int>(substringMatches.size()) < limit &&
                   (key.name.contains(needle, Qt::CaseInsensitive) ||
                    key.alias.contains(needle, Qt::CaseInsensitive))) {
            substringMatches.push_back(entries_[i]);
        }

        if (static_cast<int>(prefixMatches.size()) >= limit &&
            static_cast<int>(substringMatches.size()) >= limit) {
            break;
        }
    }

    prefixMatches.insert(prefixMatches.end(), substringMatches.begin(), substringMatches.end());
    if (static_cast<int>(prefixMatches.size()) > limit) {
        prefixMatches.resize(limit);
    }
    return prefixMatches;
}

QString CityDirectory::displayName(const Entry &entry)
{
    return entry.name + ", " + entry.country;
}

QString CityDirectory::foldForSearch(QString text)
{
    // U+0451 "ё" -> U+0435 "е", U+0401 "Ё" -> U+0415 "Е"
    text.replace(QChar(0x0451), QChar(0x0435));
    text.replace(QChar(0x0401), QChar(0x0415));
    return text;
}
