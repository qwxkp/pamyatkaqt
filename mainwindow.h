#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void initDatabase();
    void addButton();
    void showTaskByDate();
    void startupShow();
    void startupAnimation();
    void animateButton(QPushButton *pushbutton);
};
#endif // MAINWINDOW_H
