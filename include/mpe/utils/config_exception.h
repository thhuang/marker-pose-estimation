#ifndef MPE_UTILS_CONFIG_EXCEPTION_H_
#define MPE_UTILS_CONFIG_EXCEPTION_H_

#include <exception>
#include <string>

namespace mpe {

struct ConfigException : virtual public std::exception {
 protected:
  std::string error_message_;
    
 public:
  explicit 
  ConfigException(const std::string& msg) : error_message_(msg) {}

  virtual ~ConfigException() throw () {}

  virtual const char* what() const throw () {
    return error_message_.c_str();
  }
};

}  // namespace mpe

#endif  // MPE_UTILS_CONFIG_EXCEPTION_H_