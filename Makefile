$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system

CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST)
UTIL_CLASSES=nginx-configparser/config_parser.cc nginx-configparser/config_parser.h server/server.cc server/server.h

.PHONY: all clean
all: webserver

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc
tests: $(UTIL_CLASSES)

clean:
	rm -rf nginx-configparser/*.o nginx-configparser/config_parser nginx-configparser/config_parser_test webserver *.dSYM
