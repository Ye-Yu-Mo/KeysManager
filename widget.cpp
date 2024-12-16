#include "widget.h"
#include "ui_widget.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QtSql/QSqlError>
#include <QSqlTableModel>
#include <QDebug>
#include <QInputDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setWindowIcon(QIcon("://images/icon.png"));
    setFixedSize(700, 450);
    ui->setupUi(this);
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "错误", "无法连接到数据库");
        return;
    }
    mod = new QSqlTableModel;
    mod->setTable("keys");
    ui->tableView->setModel(mod);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->hideColumn(0);
    for (int i = 0; i < mod->columnCount(); ++i) {
        ui->tableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::connectToDatabase()
{
    if (db.isOpen()) {
        qDebug() << "数据库已经连接。";
        return true;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("keys.db");

    if (!db.open()) {
        qDebug() << "数据库打开失败：" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS keys (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                public_key TEXT NOT NULL,
                secret_key TEXT NOT NULL
            );
        )";

    if (!query.exec(createTableSQL)) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功，表已创建（如果不存在）。";
    return true;
}

void Widget::on_pushButton_clicked()
{
    QString name = ui->name->text();
    QString key = ui->key->text();
    QString src = ui->sec->text();

    if (name.isEmpty() || key.isEmpty() || src.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "所有字段都必须填写！", QMessageBox::Ok);
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO keys (name, public_key, secret_key) "
                  "VALUES (:name, :public_key, :secret_key)");
    query.bindValue(":name", name);
    query.bindValue(":public_key", key);
    query.bindValue(":secret_key", src);

    if (!query.exec()) {
        qDebug() << "插入失败：" << query.lastError().text();
        QMessageBox::critical(this, "错误", "插入失败！", QMessageBox::Ok);
        return;
    }

    QMessageBox::information(this, "提示", "插入成功！", QMessageBox::Ok);
    on_pushButton_2_clicked();
}

void Widget::on_pushButton_2_clicked()
{
    mod->setFilter("");  // 清除筛选条件
    mod->select();
}

void Widget::on_pushButton_3_clicked()
{
    QString name = ui->find->text();
    if (name.isEmpty()) {
        QMessageBox::critical(this, "错误", "查询失败！Name为空！", QMessageBox::Ok);
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM keys WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        QMessageBox::critical(this, "错误", "查询失败！", QMessageBox::Ok);
        return;
    }

    mod->setQuery(query);
}

void Widget::on_pushButton_4_clicked()
{
    QString text = QInputDialog::getText(this, "删除", "请输入删除的Name：");

    if (text.isEmpty()) {
        on_pushButton_2_clicked();
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM keys WHERE name = :name");
    query.bindValue(":name", text);

    if (!query.exec()) {
        qDebug() << "删除失败：" << query.lastError().text();
        QMessageBox::critical(this, "错误", "删除失败！", QMessageBox::Ok);
        return;
    }

    on_pushButton_2_clicked();
}
