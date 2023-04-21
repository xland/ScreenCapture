
#include "WebSocketClient.h"
#include <QDebug>
#include <QJsonDocument>

WebSocketClient::WebSocketClient(QObject *parent,int id)
: QObject{parent}
{
    connect(&wsClient, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(&wsClient, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    auto url = QUrl("ws://127.0.0.1:57806/?ws_"+QString::number(id));
//    auto url = QUrl("ws://127.0.0.1:56622/");
    wsClient.open(url);
}

WebSocketClient::~WebSocketClient()
{

}

void WebSocketClient::SendMessage(QJsonObject& jo)
{
    QJsonDocument doc(jo);
    auto byteArr = doc.toJson(QJsonDocument::Compact);
    wsClient.sendBinaryMessage(byteArr);
    wsClient.flush();
}

void WebSocketClient::onConnected()
{
    connect(&wsClient, &QWebSocket::textMessageReceived, this, &WebSocketClient::onMessageReceived);

}
void WebSocketClient::onDisconnected()
{
    qDebug()<<"disconnect";
}
void WebSocketClient::onMessageReceived(QString message){
    QJsonDocument jd = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jo = jd.object();
    qDebug()<<"from electron"<<jo.value("message").toString();
    ready = true;
}

