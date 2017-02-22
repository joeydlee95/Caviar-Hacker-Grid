#include "webserver_options.h"
#include <string>
#include <utility>
#include <map>
#include <numeric>

std::string WebserverOptions::ToString() const{
  std::string options_string;
  for(auto& vals : *options_) {
    options_string.append(vals.first);
    options_string.append(": ");
    for(auto& token : vals.second) {
      options_string.append(token);
      options_string.append(" ");
    }
    options_string.append("\n");
  }
  return options_string;
}

WebserverOptions::WebserverOptions(std::shared_ptr<NginxConfig> const &statement, std::map<std::string, std::vector<std::string> >* options) {
    for(const auto& s : statement->children_) {
      std::string key = s->tokens_[0];
      std::vector<std::string> vals = std::vector<std::string>(s->tokens_.begin() + 1, s->tokens_.end());
      options_->insert(std::make_pair(key, vals));
    }
}