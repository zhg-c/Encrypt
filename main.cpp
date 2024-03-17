#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Dialog w;
    if(w.Login()){
        w.show();
        return a.exec();
    }
    else{
        return 0;
    }
}
