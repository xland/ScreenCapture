#pragma once
#include <QObject>
class Logger : public QObject
{
	Q_OBJECT
public:
	Logger(QObject* parent = nullptr);
	~Logger();
	static void logProcess(QtMsgType type, const QMessageLogContext& context, const QString& msg);
};

