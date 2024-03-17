#include "dialog.h"
#include "ui_dialog.h"
#include "database.h"
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
    ui(new Ui::Dialog)
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
        if(true || QDateTime::currentDateTime().daysTo(QDateTime::fromTime_t(Time)) >= Days)
        {
            Database::GetInstance().DelPwds(true);
            QMessageBox::warning(this,"login","the probation period has arrived");
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
            Pwd = QInputDialog::getText(nullptr,"login","please enter the password",QLineEdit::Password,"",&Ok);
            if(!Ok)
            {
                break;
            }
            if(Pwd.isEmpty())
            {
                QMessageBox::warning(nullptr,"prompt","please enter the password");
            }
            else{
                bool bExist = false;
                if(Database::GetInstance().SelPwd(Pwd,bExist) && bExist)
                {
                    Database::GetInstance().UpdateDelPwdStatus(Pwd);
                    break;
                }
                if(QCryptographicHash::hash(Pwd.toStdString().c_str(),QCryptographicHash::Md5).toHex() == MD5_NUM)
                {
                    bRet = true;
                    break;
                }
                else{
                    QMessageBox::warning(nullptr,"prompt","wrong password,please re-enter");
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
    qstrTmp = bRet ? "set up successfully" : "failed to set up";
    QMessageBox::information(this,"prompt",qstrTmp);
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
    Name = "current program name: \n" + Name;
    ui->label_ProgramName->setText(Name);
}

void Dialog::on_pushButton_AddPwd_clicked()
{
    QString qstrTmp;
    bool Ok = false;
    qstrTmp = QInputDialog::getText(nullptr,"add password","please enter the password",QLineEdit::Password,"",&Ok);
    if(!Ok || qstrTmp.isEmpty())
    {
        return;
    }
    qstrTmp = Database::GetInstance().InsertPwd(qstrTmp) ? "add the password successfully" : "failed to add password";
    QMessageBox::information(this,"prompt",qstrTmp);
}

void Dialog::on_pushButton_DelPwds_clicked()
{
    QString qstrTmp;
    qstrTmp = Database::GetInstance().DelPwds() ? "the password was initialized successfully" : "failed to initialize the password";
    QMessageBox::information(this,"prompt",qstrTmp);
}

void Dialog::on_pushButton_SetName_clicked()
{
    QString qstrTmp;
    bool Ok = false;
    qstrTmp = QInputDialog::getText(nullptr,"set program name","please enter the program name",QLineEdit::Normal,"",&Ok);
    if(!Ok || qstrTmp.isEmpty())
    {
        return;
    }
    qstrTmp = Database::GetInstance().InsertProgramName(qstrTmp) ? "set the program name successfully" : "failed to set program name";

    ShowProgramName();
    QMessageBox::information(this,"prompt",qstrTmp);
}