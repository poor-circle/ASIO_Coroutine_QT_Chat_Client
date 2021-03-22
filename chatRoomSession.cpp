#include "stdafx.h"
#include "ChatRoomSession.h"

#include"namespace.h"
chatRoomSession::chatRoomSession(uint64_t ID) :ID(ID), ioc(1), socket(ioc),sendingFlag(false)
{

}
awaitable<void> chatRoomSession::sendMessage()  //���͵�ǰ�����е���Ϣ
{
	try
	{
		while (!messages.empty()) //����Ϣ�ǿ�ʱ
		{  
			//������Ϣ
			uint64_t length = messages.front()->size();                  
			co_await asio::async_write(socket, asio::buffer(&length, sizeof(length)), use_awaitable);
			co_await asio::async_write(socket, asio::buffer(*messages.front()), use_awaitable);
			//������Ϣ
			messages.pop();
		}
		sendingFlag = false;//����û��Э���ڷ�����Ϣ
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
	messages.emplace(message);//�������pushһ����Ϣ
	if (sendingFlag == false && messages.empty() == false)//�����ǰ�Ѿ���Э���ڷ�����Ϣ��������
	{
		sendingFlag = true;//���򣬿�ʼ������Ϣ
		co_await sendMessage();
	}
}

asio::awaitable<void> chatRoomSession::ReceiveMessage()   //������Ϣ
{
	try
	{
		string str;
		while (true)
		{
			//��ȡ��Ϣ
			uint64_t length;
			co_await asio::async_read(socket, asio::buffer(&length, sizeof(length)), use_awaitable);   
			str.resize(length);
			co_await asio::async_read(socket, asio::buffer(str.data(), length), use_awaitable);
			//�����źź�������UI�̷߳��źţ���ʾ�յ�һ����Ϣ
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
		//���ӷ�����
		asio::connect(socket, tcp::resolver(ioc).resolve(tcp::v4(), "localhost", to_string(myRedis::defaultPort)));
		uint32_t type = 2;
		
		asio::write(this->socket, asio::buffer(&type, sizeof(type)));
		asio::write(this->socket, asio::buffer(&ID, sizeof(ID)));
		//����������Ϣ��Э��
		co_spawn(ioc, this->ReceiveMessage(), detached);
		//��ʼ����Э��
		ioc.run();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	socket.close();
	ioc.stop();
	//TODO: �޷����ӷ����������Կ��Ƿ����ź����رջỰ����
}
//�ṩ�������̵߳��õĽӿڣ��������߳������һ��TrySendЭ�̣���������Ϣ
void chatRoomSession::send(std::shared_ptr<std::string> message)
{
	co_spawn(ioc, this->TrySend(message), detached);
}
//�����������ر������߳�
chatRoomSession::~chatRoomSession()
{
	if (socket.is_open())
	{
		socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}
	ioc.stop();
}
