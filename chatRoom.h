#pragma once

#include <QWidget>
#include "ui_chatRoom.h"
#include "chatRoomSession.h"

class chatRoom : public QWidget
{
	Q_OBJECT

public:
	chatRoom(QWidget *parent = Q_NULLPTR);
	~chatRoom();

	void raw(const QString& nickname, uint64_t ID);

	void show(int Xpos, int Ypos);

private:
	Ui::chatRoom ui;
	uint64_t ID;
	QString nickname;
	QTextBrowser* textBrowser;
	QTextEdit* textEdit;
	QString html;

	std::shared_ptr<chatRoomSession> session;

	std::thread s;

	QString createChatHTML(const QString& rawText, bool  isAlignOnLeft);


private slots:

	void sendInfo();

	void ReceiveInfo(QString s);

};


