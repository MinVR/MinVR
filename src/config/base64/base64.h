#ifndef BASE64_H_
#define BASE64_H_

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif /* BASE64_H_ */
