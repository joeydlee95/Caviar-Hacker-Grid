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
using ::testing::SetArgReferee;
using ::testing::An;

class MockNginxConfig : public NginxConfig{
public:
  std::vector<std::string> find(const std::string& key) const {
    return mocked_find(key);
  }
  MOCK_CONST_METHOD1(mocked_find,std::vector<std::string>(const std::string& key));
};

TEST(ProxyHandlerTest, InitTest) {
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("host");
	set_tokens.push_back("ucla.edu");

	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(set_tokens)
				);

	EXPECT_CALL(mock_config, mocked_find("port"))
		.WillOnce(
				Return(std::vector<std::string>{})
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
	std::vector<std::string> host_tokens;
	host_tokens.push_back("host");
	host_tokens.push_back("ucla.edu");
	std::vector<std::string> port_tokens;
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
	std::vector<std::string> set_tokens;
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
	std::vector<std::string> set_tokens;
	set_tokens.push_back("port");
	set_tokens.push_back("65539"); // larger than max
	EXPECT_CALL(mock_config, mocked_find("host"))
		.WillOnce(
				Return(std::vector<std::string>{"host", "ucla.edu"})
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
		virtual std::unique_ptr<ProxyHandler> makeTestProxyHandler(std::string prefix) {
			std::unique_ptr<ProxyHandler> handler(new ProxyHandler());

			MockNginxConfig mock_config;
			std::vector<std::string> host_tokens;
			host_tokens.push_back("host");
			host_tokens.push_back("ucla.edu");
			std::vector<std::string> port_tokens;
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

	std::string ret;
	ret = handler->ExtractNonProxyUri("/proxy", "/proxy/some/path");
	EXPECT_EQ(ret, "/some/path");

	ret = handler->ExtractNonProxyUri("/proxy/", "/proxy/some/path");
	EXPECT_EQ(ret, "/some/path");
}

TEST_F(ProxyHandlerTester, ExtractNonProxyUri_empty) {
	auto handler = makeTestProxyHandler("/");
	std::string ret = handler->ExtractNonProxyUri("/", "/");
	EXPECT_EQ(ret, "/");
}


TEST_F(ProxyHandlerTester, handleRootRequest) {
	auto handler = makeTestProxyHandler("/");

	std::string reqStr = (
			"GET / HTTP/1.1\r\n"
			"User-Agent: Mozilla/1.0\r\n"
			"\r\n"
			);

	std::unique_ptr<Request> req = Request::Parse(reqStr);
	ASSERT_NE(req, nullptr);

	/*
	Response resp;
	ASSERT_EQ(handler->HandleRequest(*req, &resp), RequestHandler::OK);

	string respStr = resp.ToString();
	EXPECT_THAT(respStr, HasSubstr("200 OK"));
	EXPECT_THAT(respStr, HasSubstr(reqStr));
	*/
}

