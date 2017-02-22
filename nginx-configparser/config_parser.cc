// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "config_parser.h"

enum TokenType {
  TOKEN_TYPE_START = 0,
  TOKEN_TYPE_NORMAL = 1,
  TOKEN_TYPE_START_BLOCK = 2,
  TOKEN_TYPE_END_BLOCK = 3,
  TOKEN_TYPE_COMMENT = 4,
  TOKEN_TYPE_STATEMENT_END = 5,
  TOKEN_TYPE_EOF = 6,
  TOKEN_TYPE_ERROR = 7
};

enum TokenParserState {
  TOKEN_STATE_INITIAL_WHITESPACE = 0,
  TOKEN_STATE_SINGLE_QUOTE = 1,
  TOKEN_STATE_DOUBLE_QUOTE = 2,
  TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
  TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
};

const char* TokenTypeAsString(TokenType type);
TokenType ParseToken(std::istream* input, std::string* value);

std::string NginxConfig::ToString(int tabLevel) const {
  std::string serialized_config;

  for(const auto& child_block_ : children_) {
    if (child_block_.get() != nullptr) {
      serialized_config.append(child_block_->ToStringSubBlock(tabLevel));
    }
  }
  return serialized_config;
}


std::string NginxConfig::ToStringSubBlock(int tabLevel) const {
  std::string serialized_config;

  for (int i = 0; i < tabLevel; ++i) {
    serialized_config.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_config.append(" ");
    }
    serialized_config.append(tokens_[i]);
  }
  if(children_.size() == 0) { // no subblock
    serialized_config.append(";\n");
  }
  else {
    for(const auto& child_block_ : children_) {
      if (child_block_.get() != nullptr) {
        serialized_config.append(" {\n");
        serialized_config.append(child_block_->ToStringSubBlock(tabLevel + 1));
        for (int i = 0; i < tabLevel; ++i) {
          serialized_config.append("  ");  
        }
        serialized_config.append("}\n");
      }
      else {
        serialized_config.append(";\n");
      }
    }
  }
  return serialized_config;
}


bool  NginxConfig::find(const std::string& key, NginxConfig& value) const {
  // Finds the first instance of the string from the root level config. 
  // Expects a config token to be the second value, and returns that.
  for(const auto& child : children_) {
    if (child->tokens_[0].compare(key) == 0) {
        value = *child;
        return true;
    }
  }
  return false;
}

std::vector<std::string> NginxConfig::find(const std::string& key) const {
  // Finds the first instance of the string from the root level config. 
  // Expects a config token to be the second value, and returns that.
  std::vector<std::string> val;
  for(const auto& child : children_) {
    if (child->tokens_[0].compare(key) == 0) {
        val.insert(val.end(), child->tokens_.begin(), child->tokens_.end());
        return val;
    }
  }
  return val;
}
std::vector<std::shared_ptr<NginxConfig> > NginxConfig::findAll(const std::string& key) const {
  std::vector<std::shared_ptr<NginxConfig> > ret_config;
  for(const auto child : children_) {
    if (child.get()->tokens_[0].compare(key) == 0) {
      ret_config.emplace_back(child);
    }
  }
  return ret_config;
}

const char* TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

TokenType ParseToken(std::istream* input, std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace.
        // TODO: Maybe also define a QUOTED_STRING token type.
        // TODO: Allow for backslash-escaping within strings.
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool ParseConfig(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int block_depth = 0;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } 
    
    if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          // Creates a new entry into the parent NginxConfig.
          config_stack.top()->children_.emplace_back(
              new NginxConfig);
        }
        // Pushes a token on the first child 
        config_stack.top()->children_.back().get()->tokens_.push_back(token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      block_depth++;
      NginxConfig* const new_config = config_stack.top()->children_.back().get();
      // config_stack.top()->children_.back().reset(
      //     new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END && 
          last_token_type != TOKEN_TYPE_END_BLOCK     &&
          last_token_type != TOKEN_TYPE_START_BLOCK
          ) {
        // Error.
        break;
      }
      config_stack.pop();
      block_depth--;
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK     &&
          last_token_type != TOKEN_TYPE_START) {
        // Error.
        break;
      }
      if (block_depth != 0) {
        printf ("Unmatched config blocks");
      }
      // If all blocks are matched, config valid.
      return block_depth == 0;
      
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool ParseFile(const std::string file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name.c_str());
    return false;
  }

  const bool return_value =
      ParseConfig(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
