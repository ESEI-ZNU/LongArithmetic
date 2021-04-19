#pragma once
#include <iostream>

/// <summary>
/// Exception type that is raized when parsing of number fails
/// </summary>
struct ParseException : public std::exception
{
private:
	std::string cause;

public:
	ParseException() {}

	/// <summary>
	/// Exception constructor
	/// </summary>
	/// <param name="cause">what caused an exception</param>
	ParseException(std::string cause)
	{
		this->cause = cause;
	}
	
	/// <summary>
	/// get cause of exception
	/// </summary>
	/// <returns>reason of exception</returns>
	const std::string getCause() const throw ()
	{
		return cause;
	}
};