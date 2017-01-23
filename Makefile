CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -std=c++0x
LIBRARIES=$(BOOST)
USERID=lab2_joanne_richard
UTIL_CLASSES=nginx-configparser/config_parser.cc

.PHONY: all
all: webserver

config_parser.cc: 
    g++ nginx-configparser/$@ -std=c++0x -g -Wall -c -o config_parser

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc $(LIBRARIES)

tests: 
.PHONY: clean
clean:
	rm -rf nginx-config_parser/*.o nginx-config_parser/config_parser nginx-config_parser/config_parser_test webserver
