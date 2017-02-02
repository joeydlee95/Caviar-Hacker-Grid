$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST) 
CLASSES=nginx-configparser/config_parser server/server
UTIL_CLASSES=$(CLASSES:=.cc)
TESTS=$(CLASSES:=_test.cc)

.PHONY: all clean test gcov
all: webserver

nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
server/server.cc: server/server.h

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc


libgtest.a: 
	$(CXX) $(GTEST_FLAGS) -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc	
	ar -rv libgtest.a gtest-all.o

%_test.cc: %.cc libgtest.a
	$(CXX) $(GTEST_FLAGS) -pthread $(UTIL_CLASSES) $(TESTS) $(GTEST_DIR)/src/gtest_main.cc libgtest.a $(BOOST) -o $(@:%.cc=%)

gcov: GTEST_FLAGS += -fprofile-arcs -ftest-coverage
gcov: test
	for test in $(TESTS:%_test.cc=%.cc); do gcov -r $$test; done

test: $(TESTS)
	for test in $(TESTS:%.cc=%); do ./$$test ; done


clean:
	rm -rf *.o nginx-configparser/config_parser $(CLASSES) webserver *.dSYM *.a *.gcda *.gcno *.gcov
	rm -rf nginx-configparser/*.a nginx-configparser/*.gcda nginx-configparser/*.gcno nginx-configparser/*.gcov
	rm -rf server/*.a server/*.gcda server/*.gcno server/*.gcov
