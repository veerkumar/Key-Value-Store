#include <boost/asio.hpp>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <random>
#include <time.h>
#include <mutex>
#include <fstream>

#include "utils.hpp"

using namespace boost::asio;
using ip::tcp;
using namespace std;
typedef unsigned int uint;

#define NUM_CLIENT  10
#define NUM_REQUEST 1000000
#define MAX_PAYLOAD_SIZE 1024

// Vector to store keys for get request.
vector<string> key_vec;
vector<long int> latency_values;
mutex mtx;

uint32_t get_random_number(){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_int_distribution<uint32_t> distribution(0, key_vec.size()-1);
  return distribution(generator);
}

long int calculate_latency(){
  long int sum = 0;
  int j = 0;
  for (uint i = 0; i < latency_values.size(); i++) {
    sum += latency_values[i];
    // To avoid overflow.
    if (j == 99) {
      sum =  sum/100;
      j = 0;
    }
    j++;
  }
  return (sum/j);
}

namespace Thread_helper{

  void client_(){
    // Create TCP connection with server.
    boost::asio::io_service io_service;
    // Socket creation.
    tcp::socket socket(io_service);
    // Create connection.
    socket.connect( tcp::endpoint(
          boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));
    string key ;
    for ( int i = 0; i < NUM_REQUEST/NUM_CLIENT; i++) {
      // Fetch the key to query.
      key =  key_vec[get_random_number()];
      // Start the clock.
      std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();
      boost::system::error_code error;
      boost::asio::write(socket, boost::asio::buffer(key, key.size()), error);
      if (error) {
        cout << "send failed: " << error.message() << endl;
      }
      // Getting response from the server.
      char reply[MAX_PAYLOAD_SIZE] = {0};
      /* First read the respones size. */
      boost::asio::read(socket, boost::asio::buffer(reply, HEADER_LEN), error);
      if (error && error != boost::asio::error::eof) {
        cout << "receive failed: " << error.message() << endl;
      }
      int body_len = stoi(reply);
      if (body_len > 0) {
        /* Now read the response. */
        boost::asio::read(socket, boost::asio::buffer(reply, body_len), error);
        if (error && error != boost::asio::error::eof) {
          cout << "receive failed: " << error.message() << endl;
        } else {
          //cout << " " <<reply << " Len: "<< reply_length <<endl ;
        }
      } else {
        cout << " Key not found or value was null Key:" << key << endl;
      }
      std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
      mtx.lock();
      latency_values.push_back(
          std::chrono::duration_cast<std::chrono::microseconds>
          (end - begin).count());
      mtx.unlock();
    }
    socket.close();
    return;
  }
}

int main(int argc, char *argv[]) {
  key_vec.reserve(10000);
  cout <<"Reading Input data"<<endl;
  if (argc < 2) {
    cout<<"Please add data input file"<<endl;
    exit(0);
  }
  std::ifstream file(argv[1]);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      // using printf() in all tests for consistencyS
      //cout<<line.c_str() <<endl;
      std::size_t first_space_loc = line.find(" ");
      if (first_space_loc !=std::string::npos) {
        string key = line.substr(0, first_space_loc);
        key_vec.emplace_back(key);
      }
    }
    file.close();
  }
  // Lets create threads.
  std::vector<std::thread*> threads;
  for ( int i = 0; i< NUM_CLIENT ; i++) {
    threads.push_back(new std::thread(Thread_helper::client_));
  }
  for(int i = 0; i < NUM_CLIENT; i++){
    threads[i]->join();
  }
  //Thread_helper::client_();
  cout << " Avg latency is :" << calculate_latency() << " x 10^-6"<<endl;

  return 0;
}
