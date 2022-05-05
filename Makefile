CXX = g++
CXXFLAGS = -g -Wall -std=c++17
LDFLAGS = -L/usr/local/lib -lm -ldl -lpthread -lboost_system -lstdc++

# All the cpp file in the directory should be compiled
src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, obj/%.o, $(src)) # Each cpp file will trun into an object file in the obj folder

obj2 = $(filter-out obj/kv_store_client.o,  $(obj))
obj1 = $(filter-out obj/kv_store_server.o, $(obj))

.PHONY: all
#all: Client Server
all: obj client server 

client: obj Client
Client:  obj/kv_store_client.o
		$(CXX) -g -o $@ $^ $(LDFLAGS)

server: obj Server
Server: obj/kv_store_server.o obj/key_value_store.o 
		$(CXX) -g  $^ -o $@ $(LDFLAGS)

obj:
	mkdir obj # Create a folder for the object files

# Compile the code and create object files
$(obj): obj/%.o: %.cpp
	$(CXX) -c  $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf obj Client Server *.o
