#include "request_handler.h"

std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders = nullptr;

RequestHandler* RequestHandler::CreateByName(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    return nullptr;
  }
  return (*type_and_builder->second)();
}

HandlerConfiguration::~HandlerConfiguration() {
  delete RequestHandlers;
  // delete DefaultHandler;
}

std::string HandlerConfiguration::FindLongestHandlerKey(std::string key) {
  std::string best_key = "";
  for(const auto & key_match : *RequestHandlers) {
    auto res = std::mismatch(key_match.first.begin(), key_match.first.end(), key.begin());
    if(res.first == key_match.first.end()) { //match found
      std::string tmp_key = std::string(key_match.first.begin(), res.first);
      if(tmp_key.size() > best_key.size()) { //if the match is better, that's our new key
        best_key = tmp_key;
      }
    }
  }
  return best_key;
}