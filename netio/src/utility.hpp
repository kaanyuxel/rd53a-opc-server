#pragma once

#include <system_error>
#include <sstream>
#include <atomic>

#define THROW_WITH_MSG(X, Y) do {\
	std::stringstream _MY_STRING_STREAM;		 \
	_MY_STRING_STREAM << Y;	            		 \
	throw X(_MY_STRING_STREAM.str());   		 \
} while(0)


namespace netio
{
inline void
raise_errno_exception()
{
    throw std::system_error(errno, std::generic_category());
}
}