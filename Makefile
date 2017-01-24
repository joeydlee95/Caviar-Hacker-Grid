$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system

GTEST=nginx-configparser/gooletest/googletest
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 -isystem \
	$(GTEST)/include -I$(GTEST)
LDFLAGS=$(BOOST)
UTIL_CLASSES=nginx-configparser/config_parser.o

.PHONY: all clean
all: clean webserver

config_parser.cc: 
    # g++ nginx-configparser/$@ -std=c++0x -g -Wall -c -o config_parser
	./nginx-config_parser/build.sh

webserver: webserver.cc $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc $(LIBRARIES) $(LDFLAGS) -v
tests: $(UTIL_CLASSES)
	./nginx-config_parser/build_tests.sh

clean:
	rm -rf nginx-config_parser/*.o nginx-config_parser/config_parser nginx-config_parser/config_parser_test webserver *.dSYM
