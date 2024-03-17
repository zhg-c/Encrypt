#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
public:
    bool Login();
private slots:
    void on_pushButton_Ok_clicked();
    void on_pushButton_AddPwd_clicked();

    void on_pushButton_DelPwds_clicked();

    void on_pushButton_SetName_clicked();

private:
    void InitDlg();
    void ShowProgramName();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
