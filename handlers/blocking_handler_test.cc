#include "gtest/gtest.h"
#include "blocking_handler.h"

#include <vector>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../nginx-configparser/config_parser.h"



// Detecting blocking based on 
// https://github.com/google/googletest/issues/348#issuecomment-235674063
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static jmp_buf jmp_env;


static void catch_alarm(int sig)
{
    longjmp(jmp_env, 1);
}


#define ASSERT_USECS(fn, usecs) { \
    const auto val = setjmp(jmp_env); \
    if (val == 0) { \
        signal(SIGALRM, catch_alarm); \
        ualarm((usecs), 0); \
        { fn; }; \
        ualarm(0, 0); \
    } else { \
        GTEST_FATAL_FAILURE_(#usecs " usecs timer tripped for " #fn); \
    } }

TEST(BlockHandlerTest, BasicTest) {
  std::string raw_request= "GET /echo HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto req = Request::Parse(raw_request);
  Response res;
  BlockingHandler h;
   
  
  ASSERT_USECS(h.HandleRequest(*req, &res), 8000);
}

