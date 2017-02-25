$CXX=g++
CXXOPTIMIZE= -O0
BOOST=-lboost_system
GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
GTEST_FLAGS=-std=c++11 -isystem $(GTEST_DIR)/include 
GMOCK_FLAGS=-isystem $(GMOCK_DIR)/include
CXXFLAGS= -g $(CXXOPTIMIZE) -Wall -Werror -pedantic -std=c++11 $(BOOST)
CLASSES=nginx-configparser/config_parser server/server webserver server/httpRequest server/httpResponse server/http filesystem/file_opener server/file_handler server/echo_handler server/request_handler
GCOV=config_parser.cc server.cc webserver.cc httpRequest.cc http.cc http_404.cc http_echo.cc http_file.cc file_opener.cc
UTIL_CLASSES=$(CLASSES:=.cc)
TESTS=$(CLASSES:=_test)

.PHONY: all clean test gcov
all: webserver

gcov: GTEST_FLAGS += -fprofile-arcs -ftest-coverage

nginx-configparser/config_parser.cc: nginx-configparser/config_parser.h
server/server.cc: server/server.h
webserver.cc: webserver.h
server/httpRequest.cc: server/httpRequest.h
filesystem/file_opener.cc: filesystem/file_opener.h
server/file_handler.cc: server/file_handler.h
server/http.cc: server/http.h
server/httpResponse.cc: server/httpResponse.h
server/echo_handler.cc: server/echo_handler.h

webserver: $(UTIL_CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) main.cc


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
<<<<<<< HEAD
	rm -rf *.o nginx-configparser/config_parser $(CLASSES) webserver *.dSYM *.a *.gcda *.gcno *.gcov
	rm -rf nginx-configparser/*.a nginx-configparser/*.gcda nginx-configparser/*.gcno nginx-configparser/*.gcov
	rm -rf httpRequest_test webserver_test server/httpRequest_test server/http_404_test server/http_echo_test server/http_file_test server/http_test server/server_test filesystem/file_opener_test nginx-configparser/config_parser_test
	rm -rf server/*.a server/*.gcda server/*.gcno server/*.gcov
	rm -rf server/*.a server/*.gcda server/*.gcno server/*.gcov

=======
	find . -type f -iname \*.o -delete
	find . -type f -iname \*.a -delete
	find . -type f -iname \*.gcda -delete
	find . -type f -iname \*.gcno -delete
	find . -type f -iname \*.gcov -delete
	rm -rf $(CLASSES) $(TESTS) webserver *.dSYM 
>>>>>>> master
