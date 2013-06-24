#ifndef PVIZCOMM_H
#define PVIZCOMM_H

#include <string>
#include <cstdarg>
#include <vector>
#include <string>
#include <sstream>

std::string 
format (const char *fmt, ...);

std::string
vformat (const char *fmt, va_list ap);

template <class T>
extern bool from_string(T& t, 
						const std::string& s, 
						std::ios_base& (*f)(std::ios_base&) = std::dec)
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
};

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
    ss.precision(16);
	ss << t;
	return ss.str();
};

void Tokenize(const std::string& str,
			  std::vector<std::string>& tokens,
			  const std::string& delimiters);

std::string quote2string(std::vector<std::string>& tokens, int idx);

#endif // PVIZCOMM_H
