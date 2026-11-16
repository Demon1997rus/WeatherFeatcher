#pragma once

#include <QHash>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <vector>

// Local offline city database (data/cities.json, generated from GeoNames).
// Entries are sorted by population (desc); lookup works both by the display
// name and by the ASCII alias, so both "Москва" and "Moscow" are found.
// Display strings are not unique (e.g. several "Springfield, US"): string
// lookups resolve to the most populated entry, exact picks go through id.
// Matching treats "ё" as "е", so "Кельн" finds "Кёльн".
class CityDirectory {
public:
    struct Entry {
        QString name;        // Display name (Russian when available)
        QString alias;       // ASCII name (may equal name)
        QString country;     // ISO country code
        QPointF coordinates; // x = latitude, y = longitude
        int id = -1;         // Position in the directory, valid for entryAt()
    };

    bool load(const QString &filePath);

    // Strings "Name, CC" for the completion model, most populated first
    const QStringList &displayNames() const;

    // Exact lookup by display string ("Москва, RU" or alias "Moscow, RU");
    // for duplicate strings the most populated entry wins
    bool find(const QString &displayName, Entry &out) const;

    // Lookup by Entry::id; false for an id this directory did not hand out
    bool entryAt(int id, Entry &out) const;

    // Best case-insensitive substring match; the most populated city wins
    bool bestMatch(const QString &text, Entry &out) const;

    // Top matches for live search: prefix matches first, then substring
    // matches; population order is preserved inside each group
    std::vector<Entry> search(const QString &text, int limit = 10) const;

    // Display string for an entry: "Name, CC"
    static QString displayName(const Entry &entry);

private:
    // Name and alias of an entry normalized for matching (see foldForSearch)
    struct SearchKey {
        QString name;
        QString alias;
    };

    // Normalization for matching: "ё" -> "е", case is left to the comparison
    static QString foldForSearch(QString text);

    std::vector<Entry> entries_;
    std::vector<SearchKey> searchKeys_; // parallel to entries_
    QStringList displayNames_;
    QHash<QString, int> indexByName_; // folded display/alias string -> index in entries_
};
