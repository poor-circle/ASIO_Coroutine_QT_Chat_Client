#include "stdafx.h"
#include "chatRoom.h"

#pragma execution_character_set("utf-8")
//初始化聊天室
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

//按下click按钮后执行的函数
void chatRoom::sendInfo()
{
	html.append(createChatHTML(textEdit->toPlainText(), false));
	textBrowser->setHtml(html);
	textBrowser->moveCursor(QTextCursor::End);
	//调用session的send函数，网络线程开始发送数据
	session->send(std::make_shared<std::string>(createChatHTML(textEdit->toPlainText(), true).toStdString()));
	textEdit->clear();
	return;
}

QString chatRoom::createChatHTML(const QString& rawText, bool isAlignOnLeft)
{
	//创建一个html
	return fmt::format(R"(<span align={}><p>{:%Y-%m-%d %H:%M:%S}</p><p><strong>{}</strong>的消息</p><div><pre>{}</pre></div><span>)", 
						isAlignOnLeft?"left":"right", //左对齐还是右对齐
						std::chrono::system_clock::now(),//当前时间
						nickname.toStdString(),			//用户昵称
						rawText.toStdString()).data();	//发送的消息
}
//槽函数，接收网络线程发来的信息
void chatRoom::ReceiveInfo(QString s)
{
	//将信息写入textBrowser
	html.append(s);
	textBrowser->setHtml(html);
	textBrowser->moveCursor(QTextCursor::End);
}
//初始化聊天室
void chatRoom::raw(const QString& nickname, uint64_t ID)
{
	this->nickname = nickname;
	this->ID = ID;
	setWindowTitle(QString("聊天室:") + QString::number(ID)+ QString("  昵称:") + nickname);
	//新建网络会话
	session = std::make_shared<chatRoomSession>(ID);
	//设置信号-槽
	connect(session.get(), &chatRoomSession::ReceivedMessageSignal, this,&chatRoom::ReceiveInfo);
	//在另外一个线程（非ui线程防止阻塞）启动会话
	s=std::thread([session = this->session]() {session->startReceive(); });
	//分离线程，使其具有独立的生命周期
	s.detach();
}

void chatRoom::show(int Xpos, int Ypos)
{
	this->setWindowFlags(Qt::Window);
	//设置窗口位置
	move(Xpos, Ypos);
	//显示窗口
	QWidget::show();
}

