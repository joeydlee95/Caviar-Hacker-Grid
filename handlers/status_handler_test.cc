#include "gtest/gtest.h"
#include "status_handler.h"

#include <vector>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../nginx-configparser/config_parser.h"
#include "../server/server.h"


TEST(StatusHandlerTest, StatusConverter) {

	ServerStatus::Status status;
	status.RequestCountByURL_ = {
		{"/echo", 15},
	};
	status.ResponseCountByCode_ = {
		{200, 100},
		{404, 6}
	};
	status.RequestHandlers_ = { 
    {"/echo", "EchoHandler"},
    {"/static", "FileHandler"},
  };
	status.requests_ = 140;

	std::string html = StatusHandler::StatusToHtml(status);
	

  std::string response= "<!DOCTYPE HTML>\n<html>\n<head>Status Page</head><body bgcolor=\"FFFFFF\">\n<h1>Status Page</h1>\n<h2>Requests Received</h2>\n<p> Total Requests Received: 140</p><p> Requests broken down by URL:<ul>\n<li>/echo : 15</li>\n</ul></p>\n<p> Requests broken down by Response Code: <ul>\n<li>200 : 100</li>\n<li>404 : 6</li>\n</ul></p>\n<h2>Handlers</h2>\n<p><ul>\n<li> /echo : EchoHandler</li>\n<li> /static : FileHandler</li>\n</ul></p>\n<p> Default handler: </p></body>\n</html>\n";
  ASSERT_EQ(html, response);
}

