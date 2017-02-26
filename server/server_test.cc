#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "server.h"


TEST(ServerStatusTest, StatusTest) {

	ServerStatus status;

    status.SetDefaultHandler("foo");
    status.LogIncomingRequest("hello", 200);
    status.LogIncomingRequest("world", 404);
    status.LogIncomingRequest("world", 404);
    status.LogIncomingRequest("world", 200);

    ServerStatus::Status s = status.GetStatus();
	auto& codes = s.ResponseCountByCode_;
	ASSERT_EQ(codes.size(), 2);
	auto it = codes.begin();
	EXPECT_EQ(it->first, 200);
	EXPECT_EQ(it->second, 2);
	it++;
	EXPECT_EQ(it->first, 404);
	EXPECT_EQ(it->second, 2);

	auto& urls = s.RequestCountByURL_;
	ASSERT_EQ(urls.size(), 2);

	auto it2 = urls.begin();
	EXPECT_EQ(it2->first, std::string("hello"));
	EXPECT_EQ(it2->second, 1);
	it2++;
	EXPECT_EQ(it2->first, std::string("world"));
	EXPECT_EQ(it2->second, 3);

}
