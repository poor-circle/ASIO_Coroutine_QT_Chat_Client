#pragma once

#include "stdafx.h"
#include "ui_ChatRoomClient.h"
#include "chatRoom.h"

class ChatRoomClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatRoomClient(QWidget *parent = Q_NULLPTR);
   

private:
    Ui::ChatRoomClientClass ui;

    QStandardItemModel* model;

    std::unordered_map<uint64_t, chatRoom> rooms;

    void addChatRoomInListView(uint64_t ID, const QString& nickname);


private slots:

    void clickChatRoomInListView(QModelIndex index);

    void openChatRoom();
};
