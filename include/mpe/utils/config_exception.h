#ifndef MPE_UTILS_CONFIG_EXCEPTION_H_
#define MPE_UTILS_CONFIG_EXCEPTION_H_

#include <exception>
#include <string>

namespace mpe {

struct ConfigException : virtual public std::exception {
 protected:
  std::string _error_message;
    
 public:
  explicit 
  ConfigException(const std::string& msg) : _error_message(msg) {}

  virtual ~ConfigException() throw () {}

  virtual const char* what() const throw () {
    return _error_message.c_str();
  }
};

}  // namespace mpe

#endif  // MPE_UTILS_CONFIG_EXCEPTION_H_