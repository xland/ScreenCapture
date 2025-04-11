#pragma once

#include <QObject>
#include <QMap>

class Lang  : public QObject
{
	Q_OBJECT

public:
	Lang(QObject *parent);
	~Lang();
	static QString get(const QString& name);
	static void init(const QString& langType="ZhCn");
private:
	void initZhCn();
	void initEn();
	QMap<QString, QString> dic;
};
