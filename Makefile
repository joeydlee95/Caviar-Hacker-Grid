$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST) 
UTIL_CLASSES=nginx-configparser/config_parser.cc server/server.cc
TESTS=nginx-configparser/config_parser_test server/server_test

.PHONY: all clean test
all: webserver

nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
server/server.cc: server/server.h

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc


libgtest.a: 
	$(CXX) $(GTEST_FLAGS) -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc	
	ar -rv libgtest.a gtest-all.o

%_test: %.cc libgtest.a
	$(CXX)  $(GTEST_FLAGS) -pthread $(BOOST) $(UTIL_CLASSES) $(TESTS:=.cc) $(GTEST_DIR)/src/gtest_main.cc libgtest.a -o $@
# sh nginx-configparser/build_tests.sh
# ./$(TESTS)

test: $(TESTS)
	for test in $(TESTS); do ./$$test ; done



clean:
	rm -rf *.o nginx-configparser/config_parser $(TESTS) webserver *.dSYM *.a
