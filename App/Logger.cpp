#include <QMessageBox>
#include "Logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"


Logger::Logger(QObject* parent)
{
	qInstallMessageHandler(&Logger::logProcess);
	auto logger = spdlog::rotating_logger_mt("logger", "Logs/log.txt", 1024000, 2); //1000K,最多2个文件
	spdlog::set_default_logger(logger);
	spdlog::set_level(spdlog::level::trace);
	spdlog::flush_on(spdlog::level::trace);
	spdlog::info("start log");
}

Logger::~Logger()
{
}

void Logger::logProcess(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	
	if (type == QtMsgType::QtFatalMsg)
	{
		spdlog::error(msg.toStdString());
		QMessageBox::critical(nullptr, "App Error", msg);
		abort();
	}
	else if (type == QtMsgType::QtWarningMsg)
	{
		spdlog::warn(msg.toStdString());
	}
	else
	{
		spdlog::info(msg.toStdString());
	}
}
