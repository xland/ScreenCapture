
#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H


#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

class WebSocketClient: public QObject
{
    Q_OBJECT
public:
    ~WebSocketClient();
    explicit WebSocketClient(QObject *parent,int id);
    void SendMessage(QJsonObject& jo);
    bool ready = false;
private:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(QString message);
    QWebSocket wsClient;
};

#endif // WEBSOCKETCLIENT_H
