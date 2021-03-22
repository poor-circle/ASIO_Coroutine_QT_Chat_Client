#include "stdafx.h"
#include "chatRoom.h"

#pragma execution_character_set("utf-8")
//��ʼ��������
chatRoom::chatRoom(QWidget *parent)
	: QWidget(parent),nickname("noname"),ID(0)
{
	ui.setupUi(this);
	textBrowser=findChild<QTextBrowser*>("textBrowser");
	textBrowser->setOpenExternalLinks(true);
	textBrowser->setOpenLinks(true);
	textBrowser->setHtml(html);
	textEdit=findChild<QTextEdit*>("textEdit");


}



chatRoom::~chatRoom()
{
	return;
}

//����click��ť��ִ�еĺ���
void chatRoom::sendInfo()
{
	html.append(createChatHTML(textEdit->toPlainText(), false));
	textBrowser->setHtml(html);
	textBrowser->moveCursor(QTextCursor::End);
	//����session��send�����������߳̿�ʼ��������
	session->send(std::make_shared<std::string>(createChatHTML(textEdit->toPlainText(), true).toStdString()));
	textEdit->clear();
	return;
}

QString chatRoom::createChatHTML(const QString& rawText, bool isAlignOnLeft)
{
	//����һ��html
	return fmt::format(R"(<span align={}><p>{:%Y-%m-%d %H:%M:%S}</p><p><strong>{}</strong>����Ϣ</p><div><pre>{}</pre></div><span>)", 
						isAlignOnLeft?"left":"right", //����뻹���Ҷ���
						std::chrono::system_clock::now(),//��ǰʱ��
						nickname.toStdString(),			//�û��ǳ�
						rawText.toStdString()).data();	//���͵���Ϣ
}
//�ۺ��������������̷߳�������Ϣ
void chatRoom::ReceiveInfo(QString s)
{
	//����Ϣд��textBrowser
	html.append(s);
	textBrowser->setHtml(html);
	textBrowser->moveCursor(QTextCursor::End);
}
//��ʼ��������
void chatRoom::raw(const QString& nickname, uint64_t ID)
{
	this->nickname = nickname;
	this->ID = ID;
	setWindowTitle(QString("������:") + QString::number(ID)+ QString("  �ǳ�:") + nickname);
	//�½�����Ự
	session = std::make_shared<chatRoomSession>(ID);
	//�����ź�-��
	connect(session.get(), &chatRoomSession::ReceivedMessageSignal, this,&chatRoom::ReceiveInfo);
	//������һ���̣߳���ui�̷߳�ֹ�����������Ự
	s=std::thread([session = this->session]() {session->startReceive(); });
	//�����̣߳�ʹ����ж�������������
	s.detach();
}

void chatRoom::show(int Xpos, int Ypos)
{
	this->setWindowFlags(Qt::Window);
	//���ô���λ��
	move(Xpos, Ypos);
	//��ʾ����
	QWidget::show();
}

