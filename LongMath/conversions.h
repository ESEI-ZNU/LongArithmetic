#pragma once
#include <iostream>
#include <vector>

int getDigit(char c)
{
	return c - 48;
}

int countDigits(long l) 
{
	int base = 10;
	int i = 1;
	for (; l /= base; i++);
	return i;
}

std::string parseString(int val, int charCount) 
{
	int base = 10;

	std::string out = std::string("0", charCount);

	while (val)
	{
		charCount--;
		char num = val % (base);
		out[charCount] = 48 + num;
		val /= base;
	}

	// fill left digits with 0
	while (charCount > 0)
	{
		charCount--;
		out[charCount] = '0';
	}
	return out;
}

std::string parseString(int val)
{
	int base = 10;
	char num;
	bool isNeg = val < 0;
	int c = countDigits(val) + isNeg;
	int i = isNeg;

	std::string out;

	if (isNeg)
	{
		out = std::string(c, '-');
	}
	else 
	{
		out = std::string(c, '0');
	}
	val = abs(val);

	while (i < c)
	{
		num = val % (base);
		out[c-i-!isNeg] = (48 + num);
		val /= base;
		i++;
	}
	return out;
}

int32_t parseInt(char* string)
{
	int32_t res = 0;
	int pow = 1;
	int base = 10;

	char* c;
	for (c = string; *c++;);
	c--; // null does not count

	while (c > string)
	{
		c--;
		res += getDigit(*c) * pow;
		pow *= base;
	}

	return res;
}

int32_t parseInt(std::string str)
{
	int32_t res = 0;
	int pow = 1;
	int base = 10;
	reverse(str.begin(), str.end());

	for (char c : str)
	{
		res += getDigit(c) * pow;
		pow *= base;
	}

	return res;
}

#define BASE_POW 2

double parseDouble(std::vector<int32_t> m_mantissa, int m_exponent, int base)
{
	double res = 0;
	int power = m_mantissa.size();
	power = -power;

	std::vector<int32_t> backwardMantissa = std::vector<int32_t>(m_mantissa);
	std::reverse(backwardMantissa.begin(), backwardMantissa.end());

	for (int32_t dig32 : backwardMantissa)
	{
		res += dig32 * pow(pow(10, BASE_POW), (power + m_exponent));
		power++;
	}

	return res;
}

template <typename T>
std::vector<T> reverse(std::vector<T> vec) 
{
	std::vector<T> reversed = std::vector<T>(vec);
	std::reverse(reversed.begin(), reversed.end());
	return reversed;
}

int32_t parseInt(int start, char* string)
{
	return parseInt(start + string);
}

void concat(char* dest, char* src)
{
	for (; *dest; dest++);
	for (; *src; *dest++ = *src++);
	*dest = 0;
}

int strlen(char* s) 
{
	int len = 0;
	for (; *s++; len++);
	return len;
}

