#include "database.h"
#include <QDateTime>
#include <QCryptographicHash>
#include <QDebug>

#define DB_PATH "ncstudio.db"

Database::Database() : m_pDb(nullptr)
{

}

Database::~Database()
{
    if(m_pDb)
    {
        sqlite3_close(m_pDb);
        m_pDb = nullptr;
    }
}

bool Database::InitDB()
{
    bool bRet = false;
    const char *Sql[] = {
        "CREATE TABLE IF NOT EXISTS exprie(id INTEGER PRIMARY KEY AUTOINCREMENT,time INTEGER ,days INT ,exittime INTEGER ,verify TEXT)",
        "CREATE TABLE IF NOT EXISTS password(exprieid INT PRIMARY KEY,pwd TEXT,isdelete INT DEFAULT 0)",
        "CREATE TABLE IF NOT EXISTS programname(id INT PRIMARY KEY,name TEXT)",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do
    {
        if(sqlite3_open(DB_PATH,&m_pDb) != SQLITE_OK)
        {
            break;
        }
        int Res = SQLITE_ERROR;
        for(int i = 0;Sql[i];i++){
            if((Res = sqlite3_exec(m_pDb,Sql[i],nullptr,nullptr,nullptr)) != SQLITE_OK)
            {
                break;
            }
        }
        if(Res != SQLITE_OK)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
        pStmt = nullptr;
    }
    return bRet;
}

bool Database::SelDate(qint64 &Time,int &Days)
{
    QString qstrTmp,Verify;
    qint64 ExitTime = 0;
    bool bRet = false;
    const char *Sql[] = {
        "SELECT a.time,a.days,a.exittime,verify FROM exprie AS a,password AS b WHERE a.id=b.exprieid AND b.isdelete=1",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Res = SQLITE_ERROR;
        while((Res = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            int Index = 0;
            Time = sqlite3_column_int64(pStmt,Index++);
            Days = sqlite3_column_int(pStmt,Index++);
            ExitTime = sqlite3_column_int64(pStmt,Index++);
            Verify = (const char *)sqlite3_column_text(pStmt,Index++);
        }
        if(Res != SQLITE_DONE)
        {
            break;
        }
        if(QDateTime::currentDateTime().toTime_t() < ExitTime)
        {
            break;
        }
        QByteArray Bytes = QCryptographicHash::hash(QString::number(Time + Days).toStdString().c_str(),QCryptographicHash::Md5).toHex();
        Bytes2Str(Bytes,qstrTmp);
        if(Verify != qstrTmp)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::SelAllDates(std::vector<std::tuple<qint64, int,int> > &vDates)
{
    bool bRet = false;
    const char *Sql[] = {
        "SELECT time,days, id FROM exprie ORDER BY time DESC",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Res = SQLITE_ERROR;
        while((Res = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            std::tuple<qint64, int,int> Date;
            int Index = 0;
            std::get<0>(Date) = sqlite3_column_int64(pStmt,Index++);
            std::get<1>(Date) = sqlite3_column_int(pStmt,Index++);
            std::get<2>(Date) = sqlite3_column_int(pStmt,Index++);
            vDates.push_back(Date);
        }
        if(Res != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::InsertDate(qint64 Time,int Days)
{
    QString qstrTmp;
    QByteArray Bytes = QCryptographicHash::hash(QString::number(Time + Days).toStdString().c_str(),QCryptographicHash::Md5).toHex();
    Bytes2Str(Bytes,qstrTmp);
    bool bRet = false;
    const char *Sql[] = {
        "INSERT INTO exprie (time,days,exittime,verify) VALUES(@1,@2,@3,@4)",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Index = 1;
        if(sqlite3_bind_int64(pStmt,Index++,Time) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_bind_int(pStmt,Index++,Days) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_bind_int64(pStmt,Index++,QDateTime::currentDateTime().toTime_t()) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_bind_text(pStmt,Index++,qstrTmp.toStdString().c_str(),-1,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::InsertPwd(const QString &Pwd,int ExprieId)
{
    QString qstrTmp;
    QByteArray Bytes = QCryptographicHash::hash(Pwd.toStdString().c_str(),QCryptographicHash::Md5).toHex();
    Bytes2Str(Bytes,qstrTmp);
    bool bRet = false;
    const char *Sql[] = {
        "REPLACE INTO password (exprieid,pwd) VALUES(@1,@2)",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Index = 1;
        if(sqlite3_bind_int(pStmt,Index++,ExprieId) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_bind_text(pStmt,Index++,qstrTmp.toStdString().c_str(),-1,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::SelPwd(const QString &Pwd,bool &bExist)
{
    bool bRet = false;
    sqlite3_stmt *pStmt = nullptr;
    QString qstrTmp;
    const char *Sql[] = {
        "SELECT COUNT(*) FROM password WHERE pwd=@1",
        nullptr
    };
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        QByteArray Bytes = QCryptographicHash::hash(Pwd.toStdString().c_str(),QCryptographicHash::Md5).toHex();
        Bytes2Str(Bytes,qstrTmp);
        if(sqlite3_bind_text(pStmt,1,qstrTmp.toStdString().c_str(),-1,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Res = SQLITE_ERROR;
        while((Res = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            bExist = sqlite3_column_int(pStmt,0);
        }
        if(Res != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::DelPwds(bool bOnlyDelUsed)
{
    bool bRet = false;
    sqlite3_stmt *pStmt = nullptr;
    const char *Sql[] = {
        "DELETE FROM password",
        "DELETE FROM password WHERE isdelete=1",
        nullptr
    };
    do{
        if(sqlite3_prepare(m_pDb,Sql[bOnlyDelUsed ? 1 : 0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    return bRet;
}

bool Database::UpdateExitTime()
{
    bool bRet = false;
    const char *Sql = "UPDATE exprie SET exittime=@1 WHERE id=0";
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql,-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_bind_int64(pStmt,1,QDateTime::currentDateTime().toTime_t()) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::InsertProgramName(const QString &Name)
{
    bool bRet = false;
    const char *Sql[] = {
        "REPLACE INTO programname (id,name) VALUES(0,@1)",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Index = 1;
        if(sqlite3_bind_text(pStmt,Index++,Name.toStdString().c_str(),-1,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::SelProgramName(QString &Name)
{
    bool bRet = false;
    const char *Sql[] = {
        "SELECT name FROM programname",
        nullptr
    };
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql[0],-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Res = SQLITE_ERROR;
        while((Res = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            int Index = 0;
            Name = (const char *)sqlite3_column_text(pStmt,Index++);
        }
        if(Res != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

bool Database::UpdateDelPwdStatus(const QString &Pwd,bool bDel)
{
    bool bRet = false;
    const char *Sql = "UPDATE password SET isdelete=@1 WHERE pwd=@2";
    QString qstrTmp;
    sqlite3_stmt *pStmt = nullptr;
    do{
        if(sqlite3_prepare(m_pDb,Sql,-1,&pStmt,nullptr) != SQLITE_OK)
        {
            break;
        }
        int Index = 1;
        if(sqlite3_bind_int(pStmt,Index++,bDel) != SQLITE_OK)
        {
            break;
        }
        QByteArray Bytes = QCryptographicHash::hash(Pwd.toStdString().c_str(),QCryptographicHash::Md5).toHex();
        Bytes2Str(Bytes,qstrTmp);
        if(sqlite3_bind_text(pStmt,Index++,qstrTmp.toStdString().c_str(),-1,nullptr) != SQLITE_OK)
        {
            break;
        }
        if(sqlite3_step(pStmt) != SQLITE_DONE)
        {
            break;
        }
        bRet = true;
    }while(false);
    if(pStmt)
    {
        sqlite3_finalize(pStmt);
    }
    return bRet;
}

void Database::Bytes2Str(const QByteArray &Bytes,QString &Str)
{
    for(int i = 0;i < 4 && i < Bytes.size();i++)
    {
        Str.append(QString::number(Bytes[i]));
    }
}






















