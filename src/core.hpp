#ifndef _INCLUDED_CORE_HPP
#define _INCLUDED_CORE_HPP

// Core

#include <memory>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

class Random {
public:
	static float Float() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
};


template< typename T >
class Array : public std::vector<T> {
	// empty
};


template< typename T >
class List : public std::list<T> {
	// empty
};

typedef std::string String;


struct Error {
	String	text;
	String	file;
	size_t	line;

	Error( String t )
		: text(t)
		, file("Unknown file")
		, line(0) {
		// empty
	}

	Error( String t, String f, size_t l )
		: text(t)
		, file(f)
		, line(l) {
		// empty
	}
};

class Object {
protected:
	void _addError (String text, String file, size_t line ) const {
		_errorList.push_back(Error( text, file, line));
	}

private:
	mutable List<Error> _errorList;
};
#define addError(x) _addError(x, __FILE__, __LINE__)

#endif // _INCLUDED_CORE_HPP
