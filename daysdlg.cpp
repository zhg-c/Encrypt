#include "daysdlg.h"
#include "ui_daysdlg.h"
#include <database.h>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

DaysDlg::DaysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DaysDlg)
{
    ui->setupUi(this);
    InitDlg();
}

DaysDlg::~DaysDlg()
{
    delete ui;
}

void DaysDlg::InitDlg()
{
    setWindowTitle("请选择要添加用户密码的试用期");
    ui->pushButton_Ok->setDisabled(true);
    QStringList Header;
    Header << "开始时间" << "天数";
    ui->tableWidget_Days->setColumnCount(Header.size());
    ui->tableWidget_Days->setHorizontalHeaderLabels(Header);
    std::vector<std::tuple<qint64, int,int> > vDates;
    if(Database::GetInstance().SelAllDates(vDates))
    {
        foreach (const auto &Date, vDates) {
            int Rows = ui->tableWidget_Days->rowCount();
            ui->tableWidget_Days->insertRow(Rows);
            QTableWidgetItem *pItem = new QTableWidgetItem(QDateTime::fromTime_t(std::get<0>(Date)).toString("yyyy-MM-dd hh:mm:ss"));
            pItem->setData(Qt::UserRole,std::get<2>(Date));
            ui->tableWidget_Days->setItem(Rows,0,pItem);
            ui->tableWidget_Days->setItem(Rows,1,new QTableWidgetItem(QString::number(std::get<1>(Date))));
        }
    }
}

void DaysDlg::on_pushButton_Ok_clicked()
{
    int Row = ui->tableWidget_Days->currentRow();
    if(Row != -1)
    {
        emit DateId(ui->tableWidget_Days->item(Row,0)->data(Qt::UserRole).toInt());
        QMessageBox::information(this,"提示","已选择");
    }
}

void DaysDlg::on_tableWidget_Days_itemSelectionChanged()
{
    ui->pushButton_Ok->setEnabled(ui->tableWidget_Days->currentRow() != -1);
}
