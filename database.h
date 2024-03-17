#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>
#include <QtGlobal>

class Database
{
public:
    Database();
    ~Database();
    static Database &GetInstance(){
        static Database Instance;
        return Instance;
    }

public:
    bool InitDB();
    bool SelDate(qint64 &Time,int &Days);
    bool InsertDate(qint64 Time,int Days);
    bool InsertPwd(const QString &Pwd);
    bool SelPwd(const QString &Pwd,bool &bExist);
    bool DelPwds(bool bOnlyDelUsed = false);
    bool UpdateExitTime();
    bool InsertProgramName(const QString &Name);
    bool SelProgramName(QString &Name);
    bool UpdateDelPwdStatus(const QString &Pwd,bool bDel = true);
private:
    void Bytes2Str(const QByteArray &Bytes,QString &Str);
private:
    sqlite3 *m_pDb;
};

#endif // DATABASE_H
