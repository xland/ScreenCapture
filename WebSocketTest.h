
#ifndef WEBSOCKETTEST_H
#define WEBSOCKETTEST_H


#include <QObject>
#include <QThread>
#include <QList>
#include "WebSocketClient.h"

class WebSocketTest : public QObject
{
    Q_OBJECT
public:
    ~WebSocketTest();
    static void Init(QObject *parent);
    static WebSocketTest* Get();
private:
    explicit WebSocketTest(QObject *parent = nullptr);
    QList<QThread*> threads;
    QList<WebSocketClient*> clients;

signals:

};

#endif // WEBSOCKETTEST_H
