#ifndef _KV_STORE_SERVER_H_
#define _KV_STORE_SERVER_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <fstream>
#include <iostream>
using namespace boost::asio;
using namespace std;

class con_handler : public boost::enable_shared_from_this<con_handler>
{
	private:
	  ip::tcp::socket sock;
	  std::shared_ptr<KV_store> kvs_;
	  enum { max_read_length = 36 };
	  char received_key[max_read_length] ={'\0'};

	  // enum { max_write_length = 1024 };
	  // char write_data[max_write_length];

	public:
	  typedef boost::shared_ptr<con_handler> pointer;
	  con_handler(boost::asio::io_service& io_service, std::shared_ptr<KV_store> kvs);
	// creating the pointer
	  static pointer create(boost::asio::io_service& io_service, std::shared_ptr<KV_store> kvs);
	//socket creation
	  ip::tcp::socket& socket();

	  void start();

	  void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
	  void handle_write(const boost::system::error_code& err, size_t bytes_transferred);
	};

class Server 
	{
	private:
	   ip::tcp::acceptor acceptor_;
	   std::shared_ptr<KV_store> kvs_;
	   void start_accept();


	public:
	//constructor for accepting connection from client
	  Server(boost::asio::io_service& io_service, int port, std::shared_ptr<KV_store> kvs);
	  void handle_accept(con_handler::pointer connection, const boost::system::error_code& err);
};

class kv_store_server {
	public:
		std::shared_ptr<KV_store> kv_store_obj_;
		boost::asio::io_service io_service; 
	public:
		kv_store_server();
};

#endif