#include "stdafx.h"
#include "ChatRoomClient.h"
#include <QtWidgets/QApplication>
#pragma execution_character_set("utf-8")
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatRoomClient w;

    w.show();
    return a.exec();
}
