#include "stdafx.h"
#include "ChatRoomClient.h"
#include "chatRoom.h"
#pragma execution_character_set("utf-8")

ChatRoomClient::ChatRoomClient(QWidget *parent)
    : QMainWindow(parent), model(new QStandardItemModel(0, 1, this))
{
    
    ui.setupUi(this);
    ui.centralWidget->findChild<QListView*>("listView")->setModel(model);
    
    return;
}



void ChatRoomClient::addChatRoomInListView(uint64_t ID, const QString& nickname)
{
    auto item = new QStandardItem(fmt::format("ÁÄÌìÊÒ:{}  êÇ³Æ:{}",ID, nickname.toStdString()).c_str());
    item->setData(QVariant(ID));
    model->appendRow(item);
}

void ChatRoomClient::clickChatRoomInListView(QModelIndex index)
{
    auto ID= model->itemFromIndex(index)->data().toULongLong();
    if (auto iter=rooms.find(ID);iter != rooms.end())
    {
        iter->second.show(this->x() + 50, this->y() + 50);
    }
    return;
}



void ChatRoomClient::openChatRoom()
{
    bool ok;
    auto text = QInputDialog::getText(this, tr("Ìí¼ÓÁÄÌìÊÒ"), tr("ÇëÊäÈëÁÄÌìÊÒID£º"), QLineEdit::Normal, 0, &ok);
    if (ok == false) return;
    uint64_t ID = text.toULongLong(&ok);
    if (ok == false)
    {
        QMessageBox::information(NULL, "´íÎó", "ÊäÈëµÄIDºÅÓÐÎó!\n");
        return;
    }
    text = QInputDialog::getText(this, tr("Ìí¼ÓÁÄÌìÊÒ"), tr("ÇëÊäÈëÄãµÄêÇ³Æ£º"), QLineEdit::Normal, 0, &ok);
    if (ok == false) return;
  
    if (auto iter = rooms.find(ID); iter == rooms.end())
    {
        rooms.emplace(ID, this).first->second.raw(text,ID);
        addChatRoomInListView(ID, text);
    }
}
