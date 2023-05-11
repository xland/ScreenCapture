
#ifndef PATHITEM_H
#define PATHITEM_H


#include <QObject>


class PathItem : public QObject
{
    Q_OBJECT
public:
    explicit PathItem(QObject* parent = nullptr);
    bool visible = true;

signals:

};

#endif // PATHITEM_H
