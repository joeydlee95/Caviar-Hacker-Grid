$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
GMOCK_FLAGS=-isystem $(GMOCK_DIR)/include
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -std=c++11 $(BOOST) 
UTIL_CLASSES=nginx-configparser/config_parser.cc server/server.cc
TESTS=nginx-configparser/config_parser_test server/server_test
.PHONY: all clean test gcov
all: webserver


nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
server/server.cc: server/server.h
webserver.cc: webserver.h
webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cc main.cc


libgtest.a: 
	$(CXX) $(GTEST_FLAGS) -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc	
	ar -rv libgtest.a gtest-all.o

%_test: %.cc %_test.cc libgtest.a
	$(CXX) $(GTEST_FLAGS) -pthread $(UTIL_CLASSES) $(TESTS:=.cc) $(GTEST_DIR)/src/gtest_main.cc libgtest.a $(BOOST) -fprofile-arcs -ftest-coverage -o $@

gcov: test
	for test in $(TESTS:%_test=%.cc); do gcov -r $$test; done

test: $(TESTS) mock_webserver
	for test in $(TESTS); do ./$$test ; done
	./webserver_test
	python integration_test.py

libgmock.a:
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

mock_webserver: libgmock.a 
	$(CXX) $(GTEST_FLAGS) $(GMOCK_FLAGS) $(UTIL_CLASSES) webserver.cc -pthread webserver_test.cc $(GMOCK_DIR)/src/gmock_main.cc libgmock.a $(BOOST) -o webserver_test
	./webserver_test
	python integration_test.py

clean:
	rm -rf *.o nginx-configparser/config_parser $(TESTS) webserver *.dSYM *.a *.gcda *.gcno *.gcov
