#include "citysearchbox.h"

#include <QAction>
#include <QKeyEvent>
#include <QListWidget>
#include <QStyle>
#include <QTimer>

namespace {
constexpr int searchDebounceMs = 200; // Search after typing pauses
constexpr int maxSearchResults = 10;
} // namespace

CitySearchBox::CitySearchBox(const CityDirectory &cities, QWidget *parent) :
    QLineEdit(parent), cities(cities)
{
    setObjectName("search");
    setPlaceholderText("Начните вводить город...");
    setClearButtonEnabled(true);

    debounce = new QTimer(this);
    debounce->setSingleShot(true);
    debounce->setInterval(searchDebounceMs);

    // Drop-down affordance: opens the top-cities list on click
    auto *dropdownAction =
        addAction(style()->standardIcon(QStyle::SP_ArrowDown), TrailingPosition);
    connect(dropdownAction, &QAction::triggered, this, [this]() {
        if (popup && popup->isVisible()) {
            hidePopup();
        } else {
            setFocus();
            showBrowseList();
        }
    });

    connect(this, &QLineEdit::textChanged, this, [this](const QString &) {
        if (!suppressSearch) {
            debounce->start();
        }
    });
    connect(debounce, &QTimer::timeout, this, &CitySearchBox::performSearch);
}

void CitySearchBox::setCityDisplay(const CityDirectory::Entry &entry)
{
    suppressSearch = true;
    setText(CityDirectory::displayName(entry));
    suppressSearch = false;
}

void CitySearchBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && popup && popup->isVisible()) {
        hidePopup();
        return;
    }

    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
        if (!popup || !popup->isVisible()) {
            showBrowseList();
            return;
        }
        const int step = (event->key() == Qt::Key_Down) ? 1 : -1;
        const int next = qBound(0, popup->currentRow() + step, popup->count() - 1);
        popup->setCurrentRow(next);
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Enter with an open popup activates the highlighted row
        if (popup && popup->isVisible() && popup->currentRow() >= 0) {
            const QString displayName = popup->currentItem()->text();
            CityDirectory::Entry entry;
            if (cities.find(displayName, entry)) {
                acceptEntry(entry);
            }
            return;
        }

        const QString text = this->text().trimmed();
        if (text.isEmpty()) {
            return;
        }
        CityDirectory::Entry entry;
        if (cities.find(text, entry) || cities.bestMatch(text, entry)) {
            acceptEntry(entry);
        } else {
            emit searchFailed(text);
        }
        return;
    }

    QLineEdit::keyPressEvent(event);
}

void CitySearchBox::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    // Focusing an empty field shows the top cities right away; deferred so the
    // popup appears after the focus machinery settles (see Qt docs on queued calls)
    if (text().trimmed().isEmpty()) {
        QTimer::singleShot(0, this, &CitySearchBox::showBrowseList);
    }
}

void CitySearchBox::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    // Delayed hide so a click on a popup row is processed first
    QTimer::singleShot(150, this, &CitySearchBox::hidePopup);
}

void CitySearchBox::performSearch()
{
    const QString text = this->text().trimmed();
    if (text.isEmpty()) {
        showBrowseList();
        return;
    }

    const auto results = cities.search(text, maxSearchResults);
    if (results.empty()) {
        hidePopup();
        emit searchFailed(text);
        return;
    }

    showResults(results);
}

void CitySearchBox::showBrowseList()
{
    // Browse mode: top cities by population, regardless of the field text
    showResults(cities.search("", maxSearchResults));
}

void CitySearchBox::showResults(const std::vector<CityDirectory::Entry> &results)
{
    if (!popup) {
        popup = new QListWidget(window());
        popup->setObjectName("searchPopup");
        popup->setMouseTracking(true);
        popup->setStyleSheet(R"(
            QListWidget#searchPopup {
                background-color: #20303c;
                color: #e8eef2;
                border: 1px solid rgba(255, 255, 255, 40);
                border-radius: 8px;
                padding: 4px;
                outline: none;
                font-size: 14px;
            }
            QListWidget#searchPopup::item {
                padding: 7px 10px;
                border-radius: 5px;
            }
            QListWidget#searchPopup::item:selected {
                background-color: #2c5364;
            }
            QListWidget#searchPopup::item:hover {
                background-color: rgba(44, 83, 100, 120);
            }
        )");
        connect(popup, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            CityDirectory::Entry entry;
            if (cities.find(item->text(), entry)) {
                acceptEntry(entry);
            }
        });
    }

    popup->clear();
    for (const auto &entry : results) {
        popup->addItem(CityDirectory::displayName(entry));
    }

    // Popup right under the search field, same width
    const QPoint fieldPos = mapTo(window(), QPoint(0, height() + 4));
    const int rowHeight = 32;
    popup->setGeometry(fieldPos.x(), fieldPos.y(), width(),
                       rowHeight * static_cast<int>(results.size()) + 8);
    popup->setCurrentRow(0);
    popup->show();
    popup->raise();
}

void CitySearchBox::hidePopup()
{
    if (popup) {
        popup->hide();
    }
}

void CitySearchBox::acceptEntry(const CityDirectory::Entry &entry)
{
    setCityDisplay(entry);
    hidePopup();
    emit citySelected(entry);
}
