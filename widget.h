#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QSqlTableModel>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    bool connectToDatabase();
    ~Widget();

private slots:
    void on_pushButton_clicked();

    // void on_tableWidget_cellActivated(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;
    QSqlDatabase db;
    QSqlTableModel * mod;
};
#endif // WIDGET_H
