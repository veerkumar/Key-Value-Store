#include "key_value_store.h"
#include "kv_store_server.h"
#include "utils.hpp"

#define PORT 1234
#define MSG_SIZE 10

con_handler::con_handler(boost::asio::io_service& io_service,
    std::shared_ptr<KV_store> kvs):
  sock(io_service), kvs_(kvs){
}

typedef boost::shared_ptr<con_handler> pointer;

// creating the pointer
pointer con_handler::create(boost::asio::io_service& io_service,
    std::shared_ptr<KV_store> kvs)
{
  return pointer(new con_handler(io_service, kvs));
}
//socket creation
ip::tcp::socket& con_handler::socket()
{
  return sock;
}

void con_handler::start()
{
  async_read( sock,
      boost::asio::buffer(received_key, max_read_length),
      boost::bind(&con_handler::handle_read,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void con_handler::handle_read(const boost::system::error_code& err,
  size_t bytes_transferred)
{
  if (!err) {
    //cout << received_key << endl;
    optional<string> return_value =
      kvs_->get_value(string(received_key, 36));
    string value = "";
    if (return_value != nullopt) {
      value = *return_value;
    }
    response_t* resp = new response_t(value);
    async_write(sock,
        boost::asio::buffer(resp->data(), resp->length()),
        boost::bind(&con_handler::handle_write,
          shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    delete resp;
    //cout <<"Returned the value:" << value <<endl;
  } else {
    if (err != boost::asio::error::eof) {
      std::cerr << "Error: " << err.message() << std::endl;
    }
    sock.close();
  }
}

void con_handler::handle_write(const boost::system::error_code& err,
    size_t bytes_transferred) {
  if (!err) {
    start();
  } else {
    std::cerr << "error: " << err.message() << endl;
    sock.close();
  }
}

// Definations of Server class.
void Server::start_accept()
{
  // Socket.
  con_handler::pointer connection =
    con_handler::create(acceptor_.get_io_service(), kvs_);

  // Asynchronous accept operation and wait for a new connection.
  acceptor_.async_accept(connection->socket(),
      boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
}

// Constructor for accepting connection from client.
Server::Server(boost::asio::io_service& io_service, int port,
    std::shared_ptr<KV_store> kvs): acceptor_(io_service,
      ip::tcp::endpoint(ip::tcp::v4(), port)), kvs_(kvs)
{
  start_accept();
}

void Server::handle_accept(con_handler::pointer connection,
    const boost::system::error_code& err)
{
  if (!err) {
    connection->start();
  }
  start_accept();
}

kv_store_server::kv_store_server() {
  // Create key value store.
  kv_store_obj_ = KV_store::get_kv_obj();
  cout <<"Initializing key value store....."<<endl;
}

int main(int argc, char *argv[])
{
  kv_store_server* kv_server_obj_ =  new kv_store_server();
  cout <<"Reading Input data file.";
  if (argc < 2) {
    cout<<"Please add data input file"<<endl;
    exit(0);
  }
  std::ifstream file(argv[1]);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::size_t first_space_loc = line.find(" ");
      if (first_space_loc !=std::string::npos) {
        string value = line.substr(first_space_loc+1);
        string key = line.substr(0, first_space_loc);
        kv_server_obj_->kv_store_obj_->insert_value(key, value);
      }
    }
    file.close();
  }
  cout <<" Total size: " << kv_server_obj_->kv_store_obj_->get_kv_map_size()<<endl;
  try
  {
    boost::asio::io_service io_service;
    Server server(io_service, PORT, kv_server_obj_->kv_store_obj_);
    cout << "Server is running..."<<endl;
    io_service.run();
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << endl;
  }
  return 0;
}
