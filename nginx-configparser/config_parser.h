#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>



  // The parsed representation of the entire config.
  class NginxConfig {
  public:
    // The general ToString method. The top level nginxConfig has no tokens
    // And only has children, so this doesn't display
    // Unnecessarily indented blocks
    std::string ToString(int tabLevel = 0) const;
    // Recursive function that displays full brackets.
    std::string ToStringSubBlock(int tabLevel) const;

    // Finds a key and puts the value into value. Returns true if found.
    virtual bool find(const std::string& key, NginxConfig& value) const;
    // Finds a key and returns the tokens assosciated with the token
    // Children block are ignored.
    virtual std::vector<std::string> find(const std::string& key) const;
    // Finds all keys that match the key.
    virtual std::vector<std::shared_ptr<NginxConfig> > findAll(const std::string& key) const;

    std::vector<std::shared_ptr<NginxConfig> > children_;
    std::vector<std::string> tokens_;
  };

  // Parses configs into NginxConfig*
  bool ParseConfig(std::istream* config_file, NginxConfig* config);
  bool ParseFile(const std::string file_name, NginxConfig* config);
  
  


#endif

