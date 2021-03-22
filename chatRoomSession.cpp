#include "stdafx.h"
#include "ChatRoomSession.h"

#include"namespace.h"
chatRoomSession::chatRoomSession(uint64_t ID) :ID(ID), ioc(1), socket(ioc),sendingFlag(false)
{

}
awaitable<void> chatRoomSession::sendMessage()  //发送当前队列中的消息
{
	try
	{
		while (!messages.empty()) //当消息非空时
		{  
			//发送消息
			uint64_t length = messages.front()->size();                  
			co_await asio::async_write(socket, asio::buffer(&length, sizeof(length)), use_awaitable);
			co_await asio::async_write(socket, asio::buffer(*messages.front()), use_awaitable);
			//弹出消息
			messages.pop();
		}
		sendingFlag = false;//现在没有协程在发送消息
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		socket.close();
		ioc.stop();
	}
}


asio::awaitable<void> chatRoomSession::TrySend(shared_ptr<string> message)
{
	messages.emplace(message);//向队列中push一条信息
	if (sendingFlag == false && messages.empty() == false)//如果当前已经有协程在发送信息，就跳过
	{
		sendingFlag = true;//否则，开始发送信息
		co_await sendMessage();
	}
}

asio::awaitable<void> chatRoomSession::ReceiveMessage()   //接收消息
{
	try
	{
		string str;
		while (true)
		{
			//读取消息
			uint64_t length;
			co_await asio::async_read(socket, asio::buffer(&length, sizeof(length)), use_awaitable);   
			str.resize(length);
			co_await asio::async_read(socket, asio::buffer(str.data(), length), use_awaitable);
			//调用信号函数，向UI线程发信号，表示收到一条消息
			emit ReceivedMessageSignal(QString::fromStdString(str));
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		socket.close();
		ioc.stop();
	}
}
void chatRoomSession::startReceive()
{
	try
	{
		asio::signal_set mysignal(ioc, SIGINT, SIGTERM);
		mysignal.async_wait([&](auto, auto) { ioc.stop(); });
		//连接服务器
		asio::connect(socket, tcp::resolver(ioc).resolve(tcp::v4(), "localhost", to_string(myRedis::defaultPort)));
		uint32_t type = 2;
		
		asio::write(this->socket, asio::buffer(&type, sizeof(type)));
		asio::write(this->socket, asio::buffer(&ID, sizeof(ID)));
		//启动接收消息的协程
		co_spawn(ioc, this->ReceiveMessage(), detached);
		//开始运行协程
		ioc.run();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	socket.close();
	ioc.stop();
	//TODO: 无法连接服务器，可以考虑发送信号来关闭会话窗口
}
//提供给其他线程调用的接口，在网络线程中添加一个TrySend协程，负责发送信息
void chatRoomSession::send(std::shared_ptr<std::string> message)
{
	co_spawn(ioc, this->TrySend(message), detached);
}
//析构函数，关闭网络线程
chatRoomSession::~chatRoomSession()
{
	if (socket.is_open())
	{
		socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}
	ioc.stop();
}
