#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Nginx {

  // The parsed representation of the entire config.
  class NginxConfig {
  public:
    std::string ToString(int tabLevel = 0) const;
    std::string ToStringSubBlock(int tabLevel) const;

    virtual bool find(const std::string& key, NginxConfig& value) const;
    virtual std::vector<std::string> find(const std::string& key) const;
    virtual std::vector<std::shared_ptr<NginxConfig> > findAll(const std::string& key) const;

    std::vector<std::shared_ptr<NginxConfig> > children_;
    std::vector<std::string> tokens_;
  };

  // Parses configs into NginxConfig*
  bool ParseConfig(std::istream* config_file, NginxConfig* config);
  bool ParseFile(const std::string file_name, NginxConfig* config);
  
  
}

#endif

