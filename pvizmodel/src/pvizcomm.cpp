#include "pvizcomm.h"
#include <stdio.h>
#include <stdarg.h>

std::string
format (const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	std::string buf = vformat (fmt, ap);
	va_end (ap);
	return buf;
}

std::string
vformat (const char *fmt, va_list ap)
{
	// Allocate a buffer on the stack that's big enough for us almost
	// all the time.  Be prepared to allocate dynamically if it doesn't fit.
	size_t size = 1024;
	char stackbuf[1024];
	std::vector<char> dynamicbuf;
	char *buf = &stackbuf[0];
	
	while (1) {
		// Try to vsnprintf into our buffer.
		int needed = vsnprintf (buf, size, fmt, ap);
		// NB. C99 (which modern Linux and OS X follow) says vsnprintf
		// failure returns the length it would have needed.  But older
		// glibc and current Windows return -1 for failure, i.e., not
		// telling us how much was needed.
		
		if (needed <= (int)size && needed >= 0) {
			// It fit fine so we're done.
			return std::string (buf, (size_t) needed);
		}
		
		// vsnprintf reported that it wanted to write more characters
		// than we allotted.  So try again using a dynamic buffer.  This
		// doesn't happen very often if we chose our initial size well.
		size = (needed > 0) ? (needed+1) : (size*2);
		dynamicbuf.resize (size);
		buf = &dynamicbuf[0];
	}
}

/*
template <class T>
extern bool from_string(T& t, 
						const std::string& s, 
						std::ios_base& (*f)(std::ios_base&) = std::dec)
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}
*/

// Credit to Al Dev (Alavoor Vasudevan) alavoor[AT]yahoo.com
// http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
void Tokenize(const std::string& str,
			  std::vector<std::string>& tokens,
			  const std::string& delimiters = " ")
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

std::string quote2string(std::vector<std::string>& tokens, int idx)
{
	if (tokens[idx][0] == '\"')
	{
		std::string label;
		
		tokens[idx].erase(tokens[idx].find('\"'), 1);
		
		while (tokens[idx][tokens[idx].size()-1] != '\"') 
		{
			label += tokens[idx] + ' ';
			idx ++;
		};
		
		tokens[idx].erase(tokens[idx].find('\"'), 1);
		
		label += tokens[idx];
		
		return label;
	}
	else 
	{
		return tokens[idx];
	}
}
