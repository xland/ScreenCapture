#include "WebSocketTest.h"
#include <QJsonObject>

WebSocketTest* instance;

WebSocketTest::WebSocketTest(QObject *parent)
    : QObject{parent}
{
    auto size = 1;
    for(int i=0;i<size;i++){
        auto client = new WebSocketClient(this->parent(),i);
        clients.append(client);
    }
    for(int i=0;i<size;i++){
        auto thread = QThread::create([this](int i){
            while(true){
                if(!this->clients[i]->ready) {
                    QThread::msleep(60);
                    continue;
                }

                QJsonObject jo1;
                jo1.insert("messageFrom", "qt");
                jo1.insert("sendThread", i);
                jo1.insert("data","Connected messages 1111111111!!!");
                this->clients[i]->SendMessage(jo1);

                QJsonObject jo2;
                jo2.insert("messageFrom", "qt");
                jo2.insert("sendThread", i);
                jo2.insert("data", "Connected messages 2222222222!!!");
                this->clients[i]->SendMessage(jo2);
                QThread::msleep(1000*60*60);
            }
        },i);
        threads.append(thread);
        thread->start();
    }
}
WebSocketTest::~WebSocketTest()
{

}
WebSocketTest* WebSocketTest::Get()
{
     return instance;
}
void WebSocketTest::Init(QObject *parent)
{
    if(instance) return;
    instance = new WebSocketTest(parent);
}
