#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"

#define BigNum(x) BigFloat(#x)
#define basePow 2

class BigFloat
{
protected:
	bool isNeg = false;
	std::vector<std::int32_t> mantissa;
	int32_t exponent = 0;

public:

	BigFloat() {};

	BigFloat(const char* repr)
	{
		*this = repr;
	}

	BigFloat operator = (const char* repr)
	{
		// normalize string
		std::string str = preprocStr(repr);

		int digitCounter = str.size();
		int dotIdx = str.find('.');

		if (dotIdx != -1)
		{
			digitCounter = dotIdx;
			str = str.erase(dotIdx, 1);
		}

		int curPtr = 0;
		int strIdx = digitCounter % basePow;

		std::string digit = std::string(basePow, '0');

		while (curPtr < str.size())
		{
			digit[strIdx] = str[curPtr];

			if (strIdx == basePow - 1)
			{
				this->mantissa.push_back(parseInt(digit));
				digit = std::string(basePow, '0');
			}

			curPtr++;
			strIdx++;
			strIdx %= basePow;
		}

		// just in case
		this->mantissa.push_back(parseInt(digit));

		// count exponent based on dot position
		this->exponent = ceil(digitCounter / (float)basePow);
		this->normalize();

		return *this;
	}

	std::string preprocStr(const char* s)
	{
		std::string str = std::string(s);
		int idx = 0;
		bool prevIsNull = true;

		// remove all underscores and leading zeros
		while(idx < str.length())
		{
			char c = str[idx];

			if (c == '_' || (prevIsNull && c == '0'))
			{
				prevIsNull = c == '0';
				str = str.erase(idx, 1);
				idx--;
			}

			if (c != '0')
				prevIsNull = false;

			idx++;
		}
		return str;
	}

	void normalize()
	{
		// remove leading zeros and move them to exponent
		for (int32_t digit : this->mantissa) 
		{
			if (digit == 0) 
			{
				this->mantissa.erase( mantissa.begin() );
				this->exponent--;
			}
			else 
			{
				break;
			}
		}

		std::vector<int32_t> backwardMantissa = std::vector<int32_t>(mantissa);
		std::reverse(backwardMantissa.begin(), backwardMantissa.end());
		
		for (int32_t digit : backwardMantissa)
		{
			if (digit == 0) 
			{
				mantissa.pop_back();
			}
			else 
			{
				break;
			}
		}
	}

	double toDouble() 
	{
		if (exponent > 127) 
		{
			return INFINITY;
		}

		if (exponent < -127) 
		{
			return 0;
		}
		double d = parseDouble(this->mantissa, this->exponent, pow(10, basePow) );
		return d;
	}

	// todo:
	// add numeric constructor
	BigFloat(long value)
	{

	}

	BigFloat& operator-()
	{
		this->isNeg = !this->isNeg;
		return *this;
	}

	// treats numbers as unsigned btw
	BigFloat& operator+(BigFloat& obj)
	{
		if (this->isNeg && !obj.isNeg)
		{
			obj - (-*(this));
		}
		else if (!this->isNeg && obj.isNeg)
		{
			*(this) - -obj;
		}
	
		bool overflow = false;
		int base = pow(10, basePow);

		std::vector<std::int32_t> mantissa1 = this->mantissa;
		std::vector<std::int32_t> mantissa2 = obj.mantissa;

		int m1ptr = 0;
		int m2ptr = 0;

		int commaDiff = (this->exponent - obj.exponent);

		// move pointer to match commas
		if (commaDiff < 0)
			m1ptr += commaDiff;
		else 
			m2ptr += commaDiff;

		std::vector<int32_t> newMantissa;

		while (m1ptr < mantissa1.size() || m2ptr < mantissa2.size() || overflow)
		{
			int32_t digit1;
			int32_t digit2;

			if (m1ptr < 0 || m1ptr >= mantissa1.size())
				digit1 = 0;
			else
				digit1 = mantissa1[m1ptr];

			if (m2ptr < 0|| m2ptr >= mantissa2.size())
				digit2 = 0;
			else
				digit2 = mantissa2[m2ptr];

			int32_t res = digit1 + digit2 + overflow;
			//std::cout << digit1 << " + " << digit2 << " = " << res << " add overflow: " << overflow << std::endl;
			overflow = res >= base;
			
			res = res % base;
			newMantissa.push_back(res);

			m1ptr++;
			m2ptr++;
		}

		this->mantissa = newMantissa;
		return *this;
	}

	BigFloat* operator-(const BigFloat obj)
	{
		// code that will decrease this by obj
		return this;
	}

	BigFloat* operator*(const BigFloat obj)
	{
		// code that will multiply this by obj
		return this;
	}

	BigFloat* operator/(const BigFloat obj)
	{
		// code that will divide this by obj
		return this;
	}

	BigFloat* operator%(const BigFloat obj)
	{
		// code that will take remainder from dividing this by obj
		return this;
	}

	friend std::ostream& operator << (std::ostream& os, BigFloat& obj)
	{
		std::vector<int32_t> digits = obj.mantissa;

		int digCount = digits.size();
		std::string repr = "0.";
		
		for (int32_t dig : obj.mantissa)
		{
			std::string digit = std::string(parseString(dig));
			repr += digit;
		}
		repr += "(B)";
		repr += parseString(obj.exponent);
		return os << repr;
	}

	
	friend std::istream& operator >> (std::istream& out, BigFloat& obj)
	{	
		std::string repr = std::string();
		char c;
		while (1) 
		{
			out.get(c);

			if (c == 10)
				break;
			repr.push_back(c);
		}
		obj = repr.c_str();
	
		return out;
	}
};
