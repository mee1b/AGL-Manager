#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QShortcut>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
class ManagerWindow;
}
QT_END_NAMESPACE

class TopWindow;

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();

public slots:
    void onPushEcho();

signals:
    void startEchoGame();

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::ManagerWindow *ui;
    TopWindow* topWindow = nullptr;
    void keyPressEvent(QKeyEvent* event);

};
#endif // MANAGERWINDOW_H
