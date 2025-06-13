#pragma once

#include <QObject>
#include <QHash>

class Lang  : public QObject
{
	Q_OBJECT

public:
	Lang(QObject *parent);
	~Lang();
	static QString get(const QString& name);
	static void init(const QString& langType="ZhCn");

	void initZhCn();
	void initEn();
private:

	QHash<QString, QString> dic;
};
