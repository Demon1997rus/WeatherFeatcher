#pragma once

#include <QLineEdit>
#include <vector>

#include "utils/citydirectory.h"

class QListWidget;
class QTimer;

// Search line edit with a debounced live popup over the offline city database.
// Self-contained: emits citySelected() on activation and searchFailed() when
// the typed text matches nothing.
class CitySearchBox : public QLineEdit {
    Q_OBJECT
public:
    explicit CitySearchBox(const CityDirectory &cities, QWidget *parent = nullptr);

    // Sets the display text without opening the popup or emitting signals
    void setCityDisplay(const CityDirectory::Entry &entry);

signals:
    void citySelected(const CityDirectory::Entry &entry);
    void searchFailed(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void performSearch();

private:
    void showBrowseList();
    void showResults(const std::vector<CityDirectory::Entry> &results);
    void hidePopup();
    void acceptEntry(const CityDirectory::Entry &entry);

    const CityDirectory &cities;
    QListWidget *popup = nullptr;
    QTimer *debounce = nullptr;
    bool suppressSearch = false;
};
