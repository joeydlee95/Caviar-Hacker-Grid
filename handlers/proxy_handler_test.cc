#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "proxy_handler.h"

#include <vector>

#include "../http/httpRequest.h"
#include "../http/httpMutableRequest.h"
#include "../http/httpResponse.h"
#include "../nginx-configparser/config_parser.h"

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgPointee;
using ::testing::An;
using ::testing::HasSubstr;
using ::testing::DoAll;
using ::testing::InSequence;

using boost::asio::ip::tcp;
using boost::asio::streambuf;
using std::string;

class MockNginxConfig : public NginxConfig{
public:
  std::vector<string> find(const string& key) const {
    return mocked_find(key);
  }
  MOCK_CONST_METHOD1(mocked_find,std::vector<string>(const string& key));
};

TEST(ProxyHandlerTest, InitTest) {
	MockNginxConfig mock_config;
	std::vector<string> set_tokens;
	set_tokens.push_back("host");
	set_tokens.push_back("ucla.edu");

	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(set_tokens)
				);

	EXPECT_CALL(mock_config, mocked_find("port"))
		.WillOnce(
				Return(std::vector<string>{})
				);

	ProxyHandler handler;
	RequestHandler::Status ret = handler.Init("dummy", mock_config);
	EXPECT_EQ(handler.getPrefix(), "dummy");
	EXPECT_EQ(handler.getProxyHost(), "ucla.edu");
	EXPECT_EQ(handler.getProxyPort(), "80");
	EXPECT_EQ(ret, RequestHandler::OK);
}

TEST(ProxyHandlerTest, InitPortTest) {
	MockNginxConfig mock_config;
	std::vector<string> host_tokens;
	host_tokens.push_back("host");
	host_tokens.push_back("ucla.edu");
	std::vector<string> port_tokens;
	port_tokens.push_back("port");
	port_tokens.push_back("8080");

	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(host_tokens)
				);

	EXPECT_CALL(mock_config, mocked_find("port"))
		.WillOnce(
				Return(port_tokens)
				);

	ProxyHandler handler;
	RequestHandler::Status ret = handler.Init("dummy", mock_config);
	EXPECT_EQ(handler.getPrefix(), "dummy");
	EXPECT_EQ(handler.getProxyHost(), "ucla.edu");
	EXPECT_EQ(handler.getProxyPort(), "8080");
	EXPECT_EQ(ret, RequestHandler::OK);
}

TEST(ProxyHandlerTest, InitFailNoProxy) {
	MockNginxConfig mock_config;
	std::vector<string> set_tokens;
	set_tokens.push_back("host");
	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(set_tokens)
				);

	ProxyHandler handler;
	RequestHandler::Status ret = handler.Init("dummy", mock_config);

	EXPECT_EQ(ret, RequestHandler::INVALID_CONFIG);
}

TEST(ProxyHandlerTest, InitFailBadPort) {
	MockNginxConfig mock_config;
	std::vector<string> set_tokens;
	set_tokens.push_back("port");
	set_tokens.push_back("65539"); // larger than max
	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(std::vector<string>{"host", "ucla.edu"})
				);
	EXPECT_CALL(mock_config, mocked_find("port"))
		.WillOnce(
				Return(set_tokens)
				);

	ProxyHandler handler;
	RequestHandler::Status ret = handler.Init("dummy", mock_config);

	EXPECT_EQ(ret, RequestHandler::INVALID_CONFIG);
}


class ProxyHandlerTester : public ::testing::Test {
	protected:
		virtual std::unique_ptr<ProxyHandler> makeTestProxyHandler(string prefix) {
			std::unique_ptr<ProxyHandler> handler(new ProxyHandler());

			MockNginxConfig mock_config;
			std::vector<string> host_tokens;
			host_tokens.push_back("host");
			host_tokens.push_back("ucla.edu");
			std::vector<string> port_tokens;
			port_tokens.push_back("port");
			port_tokens.push_back("8080");

			EXPECT_CALL(mock_config, mocked_find("host"))
				.WillOnce(
						Return(host_tokens)
						);
			EXPECT_CALL(mock_config, mocked_find("port"))

				.WillOnce(
						Return(port_tokens)
						);

			RequestHandler::Status ret = handler->Init(prefix, mock_config);
			EXPECT_EQ(handler->getPrefix(), prefix);
			EXPECT_EQ(handler->getProxyHost(), "ucla.edu");
			EXPECT_EQ(handler->getProxyPort(), "8080");
			return handler;
		}
};

TEST_F(ProxyHandlerTester, ExtractNonProxyUri_normal) {
	auto handler = makeTestProxyHandler("/proxy");

	string ret;
	ret = handler->ExtractNonProxyUri("/proxy", "/proxy/some/path");
	EXPECT_EQ(ret, "/some/path");

	ret = handler->ExtractNonProxyUri("/proxy/", "/proxy/some/path");
	EXPECT_EQ(ret, "/some/path");
}

TEST_F(ProxyHandlerTester, ExtractNonProxyUri_empty) {
	auto handler = makeTestProxyHandler("/");
	string ret = handler->ExtractNonProxyUri("/", "/");
	EXPECT_EQ(ret, "/");
}

class MockSocketProxyHandler : public ProxyHandler {
	public:
		MOCK_METHOD3(ConnectSocketToEndpoint, void(tcp::socket* socket, string host, string port));
		MOCK_METHOD3(SocketReadUntil, boost::system::error_code(tcp::socket* socket, streambuf* buf, const string& sep));
		MOCK_METHOD3(SocketReadToEOF, boost::system::error_code(tcp::socket* socket, streambuf* buf, string* data));
		MOCK_METHOD2(WriteToSocket, void(tcp::socket* socket, const string& requestString));
		MOCK_METHOD3(ReadNextHeader, bool(std::istream* response_stream, string* header_key, string* header_value));

		static std::unique_ptr<MockSocketProxyHandler> createForTesting(string prefix, string host, string port) {
			std::unique_ptr<MockSocketProxyHandler> handler(new MockSocketProxyHandler());
			handler->m_uri_prefix_ = prefix;
			handler->m_host_path_ = host;
			handler->m_port_path_ = port;
			return handler;
		}
};

ACTION_P(AddStringToStream1, str) {
	boost::asio::streambuf* response_buf = arg1;
	std::ostream os(response_buf);
	os << str;
}

TEST(ProxyHandlerTest, handleRootRequest) {
	auto mockhandler = MockSocketProxyHandler::createForTesting("/", "localhost", "8080");

	string reqStr = (
			"GET / HTTP/1.1\r\n"
			"User-Agent: Mozilla/1.0\r\n"
			"\r\n"
			);

	EXPECT_CALL(*mockhandler, ConnectSocketToEndpoint(_, "localhost", "8080"))
		.Times(1);

	// should try to write incoming request to socket
	EXPECT_CALL(*mockhandler, WriteToSocket(_, AllOf(
					HasSubstr("GET / HTTP/1.1\r\n"),
					HasSubstr("User-Agent: Mozilla/1.0\r\n"),
					HasSubstr("\r\n\r\n")
					)));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n"))
		.WillOnce(DoAll(AddStringToStream1("HTTP/1.1 200 OK\r\n"), Return(boost::system::error_code())));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n\r\n"));

	EXPECT_CALL(*mockhandler, ReadNextHeader(_, _, _))
		.WillOnce(DoAll(
					SetArgPointee<1>("Connection"),
					SetArgPointee<2>("close"),
					Return(true)
					))
		.WillOnce(DoAll(
					SetArgPointee<1>("Content-Length"),
					SetArgPointee<2>("10"),
					Return(true)
					))
		.WillOnce(Return(false));

	EXPECT_CALL(*mockhandler, SocketReadToEOF(_, _, _))
		.WillOnce(DoAll(SetArgPointee<2>("0123456789"), Return(boost::system::error_code())));

	std::unique_ptr<Request> req = Request::Parse(reqStr);
	ASSERT_NE(req, nullptr);

	Response resp;
	ASSERT_EQ(mockhandler->HandleRequest(*req, &resp), RequestHandler::OK);

	string respStr = resp.ToString();
	EXPECT_THAT(respStr, HasSubstr("HTTP/1.1 200 OK\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Connection: close\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Content-Length: 10\r\n"));
	EXPECT_THAT(respStr, HasSubstr("\r\n\r\n"));
	EXPECT_THAT(respStr, HasSubstr("0123456789"));
}

TEST(ProxyHandlerTest, handleNonRootRequest) {
	auto mockhandler = MockSocketProxyHandler::createForTesting("/proxy", "localhost", "8080");

	string reqStr = (
			"GET /proxy/some/path HTTP/1.1\r\n"
			"User-Agent: Mozilla/1.0\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			);

	EXPECT_CALL(*mockhandler, ConnectSocketToEndpoint(_, "localhost", "8080"))
		.Times(1);

	// should try to write incoming request to socket
	EXPECT_CALL(*mockhandler, WriteToSocket(_, AllOf(
					HasSubstr("GET /some/path HTTP/1.1\r\n"),
					HasSubstr("User-Agent: Mozilla/1.0\r\n"),
					HasSubstr("Connection: close\r\n"), // overwritten by proxy
					HasSubstr("\r\n\r\n")
					)));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n"))
		.WillOnce(DoAll(AddStringToStream1("HTTP/1.1 404 Not Found\r\n"), Return(boost::system::error_code())));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n\r\n"));

	EXPECT_CALL(*mockhandler, ReadNextHeader(_, _, _))
		.WillOnce(DoAll(
					SetArgPointee<1>("Connection"),
					SetArgPointee<2>("close"),
					Return(true)
					))
		.WillOnce(DoAll(
					SetArgPointee<1>("Content-Length"),
					SetArgPointee<2>("10"),
					Return(true)
					))
		.WillOnce(Return(false));

	EXPECT_CALL(*mockhandler, SocketReadToEOF(_, _, _))
		.WillOnce(DoAll(SetArgPointee<2>("Not Found!"), Return(boost::system::error_code())));

	std::unique_ptr<Request> req = Request::Parse(reqStr);
	ASSERT_NE(req, nullptr);

	Response resp;
	ASSERT_EQ(mockhandler->HandleRequest(*req, &resp), RequestHandler::OK);

	string respStr = resp.ToString();
	EXPECT_THAT(respStr, HasSubstr("HTTP/1.1 404 Not Found\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Connection: close\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Content-Length: 10\r\n"));
	EXPECT_THAT(respStr, HasSubstr("\r\n\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Not Found!"));
}

TEST(ProxyHandlerTest, handleRequest_redirect) {
	auto mockhandler = MockSocketProxyHandler::createForTesting("/", "ucla.edu", "80");

	string reqStr = (
			"GET /file.txt HTTP/1.1\r\n"
			"User-Agent: Mozilla/1.0\r\n"
			"\r\n"
			);

	InSequence forceExpectationsToBeInOrder;

	EXPECT_CALL(*mockhandler, ConnectSocketToEndpoint(_, "ucla.edu", "80"))
		.Times(1);

	// should try to write incoming request to socket
	EXPECT_CALL(*mockhandler, WriteToSocket(_, AllOf(
					HasSubstr("GET /file.txt HTTP/1.1\r\n"),
					HasSubstr("User-Agent: Mozilla/1.0\r\n"),
					HasSubstr("\r\n\r\n")
					)));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n"))
		.WillOnce(DoAll(AddStringToStream1("HTTP/1.1 302 Found\r\n"), Return(boost::system::error_code())));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n\r\n")) ;

	// after redirect, we should try the new location with a new connection
	EXPECT_CALL(*mockhandler, ReadNextHeader(_, _, _))
		.WillOnce(DoAll(
					SetArgPointee<1>("Location"),
					SetArgPointee<2>("http://www.ucla.edu/newer/file.txt"),
					Return(true)
					));

	EXPECT_CALL(*mockhandler, ConnectSocketToEndpoint(_, "www.ucla.edu", "80"))
		.Times(1);

	// should try to write incoming request to socket
	EXPECT_CALL(*mockhandler, WriteToSocket(_, AllOf(
					HasSubstr("GET /newer/file.txt HTTP/1.1\r\n"),
					HasSubstr("User-Agent: Mozilla/1.0\r\n"),
					HasSubstr("\r\n\r\n")
					)));

	// get correct response this time
	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n"))
		.WillOnce(DoAll(AddStringToStream1("HTTP/1.1 200 OK\r\n"), Return(boost::system::error_code())));

	EXPECT_CALL(*mockhandler, SocketReadUntil(_, _, "\r\n\r\n")) ;

	EXPECT_CALL(*mockhandler, ReadNextHeader(_, _, _))
		.WillOnce(DoAll(
					SetArgPointee<1>("Connection"),
					SetArgPointee<2>("close"),
					Return(true)
					))
		.WillOnce(DoAll(
					SetArgPointee<1>("Content-Length"),
					SetArgPointee<2>("15"),
					Return(true)
					))
		.WillOnce(Return(false));

	EXPECT_CALL(*mockhandler, SocketReadToEOF(_, _, _))
		.WillOnce(DoAll(SetArgPointee<2>("<file contents>"), Return(boost::system::error_code())));

	std::unique_ptr<Request> req = Request::Parse(reqStr);
	ASSERT_NE(req, nullptr);

	Response resp;
	ASSERT_EQ(mockhandler->HandleRequest(*req, &resp), RequestHandler::OK);

	string respStr = resp.ToString();
	EXPECT_THAT(respStr, HasSubstr("HTTP/1.1 200 OK\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Connection: close\r\n"));
	EXPECT_THAT(respStr, HasSubstr("Content-Length: 15\r\n"));
	EXPECT_THAT(respStr, HasSubstr("\r\n\r\n"));
	EXPECT_THAT(respStr, HasSubstr("<file contents>"));
}

