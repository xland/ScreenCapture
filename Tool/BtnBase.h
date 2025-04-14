#pragma once

#include <QWidget>

class BtnBase : public QWidget
{
	Q_OBJECT

public:
	BtnBase(const QString& name, const QChar& icon, QWidget *parent = nullptr);
	~BtnBase();
	QChar icon;
	QString name;
	bool isHover{false};
protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
private:
};
