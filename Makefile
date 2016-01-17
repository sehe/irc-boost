CXX = g++

EXE = pudding
BUILD_DIR = build

CXXFLAGS = -std=c++11 -Wall -Wextra -Wfatal-errors -pedantic -w -Winline -fno-rtti -ggdb -D_GLIBCXX_DEBUG -lboost_system -lboost_thread -lpthread

all: $(EXE)

$(EXE): dir main.o irc.o 
	$(CXX) $(CXXFLAGS) -o $(EXE) $(BUILD_DIR)/main.o $(BUILD_DIR)/irc.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/main.o -c main.cpp

irc.o: irc.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/irc.o -c irc.cpp

dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(EXE) $(BUILD_DIR)