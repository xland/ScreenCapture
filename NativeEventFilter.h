#pragma once
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <QObject>

class NativeEventFilter:public QAbstractNativeEventFilter
{
public:
	NativeEventFilter();
	~NativeEventFilter();
	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
};
