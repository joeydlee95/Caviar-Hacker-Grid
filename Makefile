$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
GMOCK_FLAGS=-isystem $(GMOCK_DIR)/include
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -pedantic -std=c++11 $(BOOST)
CLASSES=nginx-configparser/config_parser server/server server/webserver http/httpRequest http/httpResponse http/http filesystem/file_opener handlers/file_handler handlers/echo_handler handlers/request_handler handlers/not_found_handler
GCOV=config_parser.cc server.cc webserver.cc httpRequest.cc http.cc http_404.cc http_echo.cc http_file.cc file_opener.cc
UTIL_CLASSES=$(CLASSES:=.cc)
TESTS=$(CLASSES:=_test)

.PHONY: all clean test gcov
all: webserver

gcov: GTEST_FLAGS += -fprofile-arcs -ftest-coverage

nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
filesystem/file_opener.cc: filesystem/file_opener.h
server/server.cc: server/server.h

http/http.cc: http/http.h
http/httpRequest.cc: http/httpRequest.h
http/httpResponse.cc: http/httpResponse.h

handlers/file_handler.cc: handlers/file_handler.h
handlers/echo_handler.cc: handlers/echo_handler.h
handlers/not_found_handler.cc: handlers/not_found_handler.h
server/webserver.cc: server/webserver.h

webserver: main.cc $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS)


libgtest.a: 
	$(CXX) $(GTEST_FLAGS) -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc  
	ar -rv libgtest.a gtest-all.o

libgmock.a:
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

%_test: %.cc libgtest.a libgmock.a
	$(CXX) $(GTEST_FLAGS) $(GMOCK_FLAGS) -pthread $(UTIL_CLASSES) $@.cc $(GMOCK_DIR)/src/gmock_main.cc libgmock.a $(BOOST) -o $(@:%.cc=%)


gcov: test
	for test in $(GCOV); do gcov -r $$test; done

test: $(TESTS)
	for test in $(TESTS:%.cc=%); do ./$$test ; done

integration: webserver
	python integration_test.py

clean:
	find . -type f -iname \*.o -delete
	find . -type f -iname \*.a -delete
	find . -type f -iname \*.gcda -delete
	find . -type f -iname \*.gcno -delete
	find . -type f -iname \*.gcov -delete
	rm -rf $(CLASSES) $(TESTS) webserver *.dSYM 
