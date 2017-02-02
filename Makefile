$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST) 
UTIL_CLASSES=nginx-configparser/config_parser.cc server/server.cc
TESTS=nginx-configparser/config_parser_test server/server_test

.PHONY: all clean test gcov
all: webserver

nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
server/server.cc: server/server.h

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc


libgtest.a: 
	$(CXX) $(GTEST_FLAGS) -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc	
	ar -rv libgtest.a gtest-all.o

gcov: GTEST_FLAGS += -fprofile-arcs -ftest-coverage
%_test.cc: %.cc libgtest.a
	echo "asdf"
	$(CXX) $(GTEST_FLAGS) -pthread $(UTIL_CLASSES) $(TESTS:=.cc) $(GTEST_DIR)/src/gtest_main.cc libgtest.a $(BOOST) -o $(@:%.cc=%)

gcov: test clean
	for test in $(TESTS:%_test=%.cc); do gcov -r $$test; done

test: nginx-configparser/config_parser_test.cc server/server_test.cc
	for test in $(TESTS:%.cc=%); do ./$$test ; done


clean:
	rm -rf *.o nginx-configparser/config_parser $(TESTS) webserver *.dSYM *.a *.gcda *.gcno *.gcov
