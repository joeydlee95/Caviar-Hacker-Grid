#include "gtest/gtest.h"
#include "status_handler.h"

#include <vector>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../nginx-configparser/config_parser.h"
#include "../server/server.h"


TEST(StatusHandlerTest, StatusConverter) {

	ServerStatus::Status status;
	status.port_ = 1024;
	status.RequestCountByURL_ = {
		{"/echo", 15},
	};
	status.ResponseCountByCode_ = {
		{200, 100},
		{404, 6}
	};
	status.requestHandlers_ = { "a", "b" };
	status.requests_ = 140;

	std::string html = StatusHandler::StatusToHtml(status);
	

  ASSERT_EQ(html, "asdf");
}

