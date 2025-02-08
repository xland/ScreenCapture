#pragma once

#include <QObject>

namespace Win
{
	class Box : public QObject
	{
		Q_OBJECT

	public:
		Box(QObject* parent);
		~Box();
		static void init();
	};
}

