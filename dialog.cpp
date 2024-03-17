#include "dialog.h"
#include "ui_dialog.h"
#include "database.h"
#include "daysdlg.h"
#include <QDateTime>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QProcess>
#include <QEventLoop>
#include <QIcon>

#define MD5_NUM "0a5ed43ab1e290a2356aaa745e7d7196"  //Jerusalem
#define PROGRAM_NAME "program.exe"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_ExprieId(-1)
{
    ui->setupUi(this);
    InitDlg();
}

Dialog::~Dialog()
{
    delete ui;
}

bool Dialog::Login()
{
    bool bRet = false;
    qint64 Time = 0;
    int Days = 0;
    do{
        if(!Database::GetInstance().InitDB())
        {
            break;
        }
        if(!Database::GetInstance().SelDate(Time,Days))
        {
            break;
        }
        if(!Time || !Days)
        {
            break;
        }
        if(QDateTime::currentDateTime().daysTo(QDateTime::fromTime_t(Time)) >= Days)
        {
            Database::GetInstance().DelPwds(true);
            QMessageBox::warning(this,"登录","试用期已到，请购买正版");
            break;
        }
        bRet = true;
    }while(false);
    if(bRet){
        QString Name;
        if(!Database::GetInstance().SelProgramName(Name) || Name.isEmpty())
        {
            Name = PROGRAM_NAME;
        }
        QProcess p;
        p.start(Name);
        p.waitForFinished(-1);
        Database::GetInstance().UpdateExitTime();
        exit(0);
    }
    else{
        while(true){
            bRet = false;
            QString Pwd;
            bool Ok = false;
            Pwd = QInputDialog::getText(nullptr,"登录","密码输入：",QLineEdit::Password,"",&Ok);
            if(!Ok)
            {
                break;
            }
            if(Pwd.isEmpty())
            {
                QMessageBox::warning(nullptr,"提示","请输入密码");
            }
            else{
                bool bExist = false;
                if(Database::GetInstance().SelPwd(Pwd,bExist) && bExist)
                {
                    Database::GetInstance().UpdateDelPwdStatus(Pwd);
                    qint64 Time = 0;
                    int Days = 0;
                    if(!Database::GetInstance().SelDate(Time,Days))
                    {
                        QMessageBox::warning(this,"提示","请更新您的系统时间");
                    }
                    break;
                }
                if(QCryptographicHash::hash(Pwd.toStdString().c_str(),QCryptographicHash::Md5).toHex() == MD5_NUM)
                {
                    bRet = true;
                    break;
                }
                else{
                    QMessageBox::warning(nullptr,"提示","密码错误，请重新输入");
                }
            }
        }
    }
    return bRet;
}
void Dialog::on_pushButton_Ok_clicked()
{
    bool bRet = false;
    bRet = Database::GetInstance().InsertDate(ui->dateTimeEdit_Start->dateTime().toTime_t(),ui->spinBox_Days->value());
    QString qstrTmp;
    qstrTmp = bRet ? "设置成功" : "设置失败";
    QMessageBox::information(this,"提示",qstrTmp);
}

void Dialog::InitDlg()
{
    setWindowIcon(QIcon(":/res/nc.jpg"));
    setWindowTitle("NCSTUDIO");
    ui->dateTimeEdit_Start->setDisplayFormat("yyyy-HH-dd hh:mm:ss");
    ui->dateTimeEdit_Start->setDateTime(QDateTime::currentDateTime());
    ui->label_ProgramName->setWordWrap(true);
    ShowProgramName();
}

void Dialog::ShowProgramName()
{
    QString Name;
    if(!Database::GetInstance().SelProgramName(Name) || Name.isEmpty())
    {
        Name = PROGRAM_NAME;
    }
    Name = "当前程序名称：\n" + Name;
    ui->label_ProgramName->setText(Name);
}

void Dialog::on_pushButton_AddPwd_clicked()
{
    m_ExprieId = -1;
    DaysDlg Dlg;
    connect(&Dlg,&DaysDlg::DateId,[&](int ExprieId){m_ExprieId = ExprieId;});
    Dlg.exec();
    if(m_ExprieId == -1)
    {
        return;
    }
    QString qstrTmp;
    bool Ok = false;
    qstrTmp = QInputDialog::getText(nullptr,"密码添加","请输入密码：",QLineEdit::Password,"",&Ok);
    if(!Ok || qstrTmp.isEmpty())
    {
        return;
    }
    qstrTmp = Database::GetInstance().InsertPwd(qstrTmp,m_ExprieId) ? "添加密码成功" : "添加密码失败";
    QMessageBox::information(this,"提示",qstrTmp);
}

void Dialog::on_pushButton_DelPwds_clicked()
{
    QString qstrTmp;
    qstrTmp = Database::GetInstance().DelPwds() ? "密码初始化成功" : "密码初始化失败";
    QMessageBox::information(this,"提示",qstrTmp);
}

void Dialog::on_pushButton_SetName_clicked()
{
    QString qstrTmp;
    bool Ok = false;
    qstrTmp = QInputDialog::getText(nullptr,"程序命名","请输入程序名称",QLineEdit::Normal,"",&Ok);
    if(!Ok || qstrTmp.isEmpty())
    {
        return;
    }
    qstrTmp = Database::GetInstance().InsertProgramName(qstrTmp) ? "程序命名成功" : "程序命名失败";

    ShowProgramName();
    QMessageBox::information(this,"提示",qstrTmp);
}
