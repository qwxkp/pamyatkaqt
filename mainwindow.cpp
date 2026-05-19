#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QTimer>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    initDatabase();
    startupShow();
    startupAnimation();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::addButton);
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &MainWindow::showTaskByDate);
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=](){ui->stackedWidget->setCurrentIndex(0); animateButton(ui->pushButton_2);});
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=](){ui->stackedWidget->setCurrentIndex(1); animateButton(ui->pushButton_3);});
    connect(ui->pushButton_4, &QPushButton::clicked, this, [=](){ui->stackedWidget->setCurrentIndex(3); animateButton(ui->pushButton_4);});
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=](){ui->stackedWidget->setCurrentIndex(2); animateButton(ui->pushButton_5);});




}

void MainWindow::initDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("tasks.sqlite");

    if(!db.open()){
        qDebug() << "Errordatabase" << db.lastError().text();
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS todo_list (id INTEGER PRIMARY KEY, description TEXT, date TEXT)");
    query.exec("SELECT description FROM todo_list");
    while (query.next()){
        QString text = query.value(0).toString();
        qDebug() << text;
    }
}

void MainWindow::addButton(){
    QString taskText = ui->lineEdit->text();
    QString date = ui->dateEdit->date().toString("dd.MM.yyyy");
    if(taskText.isEmpty()){
        qDebug() << "empty!";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO todo_list (description, date) VALUES (:desc, :date)");
    query.bindValue(":desc", taskText);
    query.bindValue(":date", date);

    if(query.exec()) {
        // Если в базу добавилось успешно, добавляем и на экран
        startupShow();
        ui->lineEdit->clear(); // Очищаем поле ввода для следующей задачи
        QMessageBox box;
        box.setText("успішно створено!");
        box.setIcon(QMessageBox::Icon::Information);
        box.setWindowIconText("Пам'ятка");
        box.exec();
    } else {
        qDebug() << "Ошибка добавления:" << query.lastError().text();
    }
};

void MainWindow::showTaskByDate(){
    QString selecteddate = ui->calendarWidget->selectedDate().toString("dd.MM.yyyy");
    qDebug() << selecteddate;
    ui->textEdit_2->clear();
    ui->label_datesearch->setText("Пошук за датою: "+selecteddate);
    QSqlQuery query;
    query.prepare("SELECT description FROM todo_list WHERE date = :date");
    query.bindValue(":date", selecteddate);

    if (query.exec()){
        int counter = 0;
        bool hasTasks = false;
        while(query.next()){
        QString result = query.value(0).toString();
            ui->textEdit_2->append(result);
        hasTasks = true;
        }
        if (!hasTasks){
            ui->textEdit_2->setText("Empty for today!");
        }
    }
    else{
        qDebug()<< "ошибка фильтрации" <<query.lastError().text();
    }
}

void MainWindow::startupShow(){
    QString currentdate = QDate::currentDate().toString("dd.MM.yyyy");
    qDebug() << currentdate;
    ui->textEdit->clear();
    ui->label_tdn->setText("Справи на сьогодні: "+currentdate);
    QSqlQuery query;
    query.prepare("SELECT description FROM todo_list WHERE date = :date");
    query.bindValue(":date", currentdate);

    if (query.exec()){
        int counter = 0;
        bool hasTasks = false;
        while(query.next()){
            QString result = query.value(0).toString();
            ui->textEdit->append(result);
            hasTasks = true;
        }
        if (!hasTasks){
            ui->textEdit->setText("Empty for today!");
        }
    }
    else{
        qDebug()<< "ошибка фильтрации" <<query.lastError().text();
    }
}

void MainWindow::startupAnimation(){
    this->setWindowOpacity(0.0);
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this, timer](){
        qreal currentOpacity = this->windowOpacity();

        if (currentOpacity >= 1.0){
            timer->stop();
            timer->deleteLater();
        }
        else{
            this->setWindowOpacity(currentOpacity+ 0.05);
        }
    });
    timer->start(60);

}

void MainWindow::animateButton(QPushButton *pushbutton){

    if (!pushbutton) return;

    // animation things

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(pushbutton);
    pushbutton->setGraphicsEffect(effect);

    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");

    anim->setDuration(300);       // Очень быстро (150 мс), как в Телеграме
    anim->setStartValue(0.3);     // При клике кнопка мгновенно становится полупрозрачной
    anim->setEndValue(1.0);       // И плавно возвращается в 100% видимость
    anim->setEasingCurve(QEasingCurve::OutQuad); // Приятная кривая замедления

    // Важно: анимация удалит сама себя из памяти после завершения, чтобы не было утечек
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}




MainWindow::~MainWindow()
{
    delete ui;
}
