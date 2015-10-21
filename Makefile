CXX = g++
CXXFLAGS = -Wall -ansi -g -Wno-variadic-macros

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl
endif
all: client server

client: client.cpp error.cpp physical_layer.cpp physical_layer.h network_client.cpp network_client.h data_link.h data_link.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp error.cpp physical_layer.cpp network_client.cpp data_link.cpp $(LIBS)

server: server.cpp error.cpp handle.cpp physical_layer.h physical_layer.cpp network_server.cpp network_server.h data_link.h data_link.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp error.cpp handle.cpp physical_layer.cpp network_server.cpp data_link.cpp $(LIBS)

	$(RM) *.o
clean:
	$(RM) client server
