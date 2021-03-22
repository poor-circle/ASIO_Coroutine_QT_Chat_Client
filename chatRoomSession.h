#pragma once
#include "stdafx.h"
class chatRoomSession: public QObject
{
	Q_OBJECT


	uint64_t ID;
	asio::io_context ioc;
	asio::ip::tcp::socket socket;
	std::queue<std::shared_ptr<std::string>> messages;
	asio::awaitable<void> sendMessage();
	asio::awaitable<void> TrySend(std::shared_ptr<std::string> message);
	bool sendingFlag;
	asio::awaitable<void> ReceiveMessage();

public:
	chatRoomSession(uint64_t ID);
	void startReceive();
	void send(std::shared_ptr<std::string> message);
	~chatRoomSession();
	

signals:
	void ReceivedMessageSignal(QString s);//槽函数，用于接收信息
	
};