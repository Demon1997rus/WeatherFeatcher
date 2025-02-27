#pragma once

#include <QObject>

class HttpService : public QObject {
    Q_OBJECT
public:
    explicit HttpService(QObject *parent = nullptr);
};
