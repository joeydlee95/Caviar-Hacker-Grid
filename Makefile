$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system

CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST)
UTIL_CLASSES=nginx-configparser/config_parser.cc

.PHONY: all clean
all: clean webserver


config_parser.cc: 
	$(CXX) nginx-configparser/$@ -std=c++0x -g -Wall -c -o config_parser
webserver: webserver.cc $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc $(LIBRARIES) 
tests: $(UTIL_CLASSES)

clean:
	rm -rf nginx-configparser/*.o nginx-configparser/config_parser nginx-configparser/config_parser_test webserver *.dSYM
