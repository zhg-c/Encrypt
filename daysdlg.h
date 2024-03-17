#ifndef DAYSDLG_H
#define DAYSDLG_H

#include <QDialog>

namespace Ui {
class DaysDlg;
}

class DaysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DaysDlg(QWidget *parent = 0);
    ~DaysDlg();
signals:
    void DateId(int Id);
private slots:
    void on_pushButton_Ok_clicked();

    void on_tableWidget_Days_itemSelectionChanged();

private:
    void InitDlg();

private:
    Ui::DaysDlg *ui;
};

#endif // DAYSDLG_H
