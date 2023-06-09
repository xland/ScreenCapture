
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QQuickWidget>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Q_INVOKABLE void Close();
protected:
    bool nativeEvent(const QByteArray& eventType, void *message, qintptr *result) override;
private:
    void initDesktopImage();
private:
    Ui::MainWindow *ui;
    QPixmap* desktopImage;
    int winX{0};
    int winY{0};
    int winBottom{0};
    int winRight{0};
    QQuickWidget* mainView {nullptr};
};

#endif // MAINWINDOW_H
