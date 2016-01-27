all:test

#CPPFLAGS+=-DNDEBUG
CPPFLAGS+=-std=c++11 -Wall -pedantic #-fsanitize=undefined,address -Wextra
CPPFLAGS+=-g -O3
#CPPFLAGS+=-ftemplate-depth=12800 -fconstexpr-depth=16384
CPPFLAGS+=-fconstexpr-depth=1024
#BOOST_DIR=/mnt/LARGE/modboost/
BOOST_DIR=/home/sehe/custom/boost_1_59_0
CPPFLAGS+=-isystem /home/sehe/custom/nonius/include
CPPFLAGS+=-isystem $(BOOST_DIR)
#CPPFLAGS+=-isystem /usr/include/python2.7
#CPPFLAGS+=-I /tmp/autobahn-cpp/
#CPPFLAGS+=-I /usr/include/qt4

# CPPFLAGS+=-fopenmp
CPPFLAGS+=-pthread
CPPFLAGS+=-march=native

LDFLAGS+=-L $(BOOST_DIR)/stage/lib/ -Wl,-rpath,$(BOOST_DIR)/stage/lib
LDFLAGS+=-lboost_system -lboost_regex -lboost_thread -lboost_iostreams -lboost_serialization -lboost_filesystem
LDFLAGS+=-lboost_chrono -lrt -lboost_unit_test_framework 
#LDFLAGS+=-lQtCore -lQtGui 
LDFLAGS+=$(BOOST_DIR)/stage/lib/libboost_test_exec_monitor.a
#LDFLAGS+= -lmpfr -lgmp
#LDFLAGS+= -lpython2.7 -lboost_python

CXX=g++-5
# CXX=/usr/lib/gcc-snapshot/bin/g++
# CC=/usr/lib/gcc-snapshot/bin/gcc
# CXX=clang++-3.6 -stdlib=libc++
# CC=clang

%.S:%.cpp
	$(CXX) $(CPPFLAGS) $^ -S -masm=intel -o - | egrep -v '\s*\.' | c++filt > $@

%.i:%.cpp
	$(CXX) $(CPPFLAGS) $^ -E -o $@

%.o:%.cpp
	$(CXX) $(CPPFLAGS) $^ -c -o $@

chat_server: chat_server.o

%.so:%.o
	$(CXX) $(CPPFLAGS) $^ -shared -o $@ $(LDFLAGS)

test:irc.o main.o
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)
