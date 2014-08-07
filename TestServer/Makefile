BinName = TestServer
CommonPath = ../common

IOModPath = $(CommonPath)/io
LogModPath = $(CommonPath)/log
NetWorkModPath = $(CommonPath)/network
TimeModPath = $(CommonPath)/time

NetworkUtils = $(NetWorkModPath)/epollnet.o $(NetWorkModPath)/netqueue.o
IOUtils = $(IOModPath)/FileUtil.o

COMS = $(NetworkUtils) $(IOUtils)
OBJS = ./main.o ./testservconfig.o ./testserv.o $(COMS)

CXX = g++
CXXFLAGS = -g -Wall -c 

%.o : %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

all : $(BinName)

$(BinName) : $(OBJS)
	$(CXX) -o $@ $(OBJS)  -Wall -lpthread


clean:
	-rm $(OBJS) $(BinName)
