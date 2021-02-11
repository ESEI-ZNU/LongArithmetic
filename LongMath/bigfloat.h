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
		BigFloat(const char* representation)
		{
			// last not null index
			int ptr = strlen(representation)-1;

			// decrease cuz 0 counts
			int digitPtr = basePow -1;

			// add 1 to ensure space for null
			char base1b[basePow + 1];

			// add NULL at end
			base1b[basePow] = 0;

			bool countExp = false;

			// while end not reached
			while (ptr != -1) 
			{
				bool last = ptr == 0;
				// current char of number repres
				char curr = representation[ptr];

				if (curr == '_' || curr == '\'')
				{
					ptr--;
					continue;
				}
					

				// add current char to base X number
				base1b[digitPtr] = curr;

				// if base X filled
				if ((!digitPtr && curr != '.') || last) 
				{
					// parse repr as number
					int32_t digit = parseInt(digitPtr, base1b);

					// save 
					this->mantissa.push_back(digit);

					// reset digit pointer 
					// add one to compensate next --
					digitPtr = basePow;
				}

				// does not count as char
				digitPtr-= curr != '.';
				countExp = countExp || curr == '.';
				exponent += countExp;

				// go to next char
				ptr--;
			}
		}

		double toDouble() 
		{
		}

		// todo:
		// add numeric constructor
		BigFloat(long value)
		{

		}
	
	BigFloat& operator+(BigFloat& obj)
	{
		bool overflow = false;
		int base = pow(10, basePow);

		std::vector<std::int32_t> mantissa1 = this->mantissa;
		std::vector<std::int32_t> mantissa2 = obj.mantissa;

		int m1ptr = 0;
		int m2ptr = 0;

		int commaDiff = (this->exponent - obj.exponent);

		// move pointer to match commas
		if (commaDiff < 0)
			m1ptr -= commaDiff;
		else 
			m2ptr -= commaDiff;

		std::vector<int32_t> newMantissa;

		while (m1ptr < mantissa1.size() || m2ptr < mantissa1.size() || overflow)
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
			std::cout << digit1 << " + " << digit2 << " = " << res << " add overflow: " << overflow << std::endl;
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
		std::string repr = std::string();
		
		repr[0] = 0;
		std::string z = std::string();
		for (int i = digCount-1; i >= 0; i--)
		{
			std::string str = std::string(parseString(obj.mantissa[i], basePow));
			repr += str;
			z = "0.";
			z += repr;
			z += "e-";
			z += parseString(obj.exponent);
		}
		return os << z;
	}
	/*
	friend istream& operator >> (istream& out, BigFloat& obj)
	{
		long l;
		out >> l;

		// Todo:
		// initialize object

		return out;
	}*/

};
