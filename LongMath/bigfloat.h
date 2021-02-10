#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"

#define BigNum(x) BigFloat(#x)
#define basePow 9

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

			bool stopExpCount = false;

			// while end not reached
			while (ptr != -1) 
			{
				bool last = ptr == 0;
				// current char of number repres
				char curr = representation[ptr];

				// add current char to base X number
				base1b[digitPtr] = curr;

				// if base X filled
				if (!digitPtr || last) 
				{
					// parse repr as number
					int32_t digit = parseInt(digitPtr, base1b);

					// save 
					this->mantissa.push_back(digit);

					// reset digit pointer 
					// add one to compensate next --
					digitPtr = 8 + 1;
				}

				// does not count as char
				digitPtr-= curr != '.';
				stopExpCount = stopExpCount || curr == '.';
				exponent += !stopExpCount;

				// go to next char
				ptr--;
			}
		}

		// todo:
		// add numeric constructor
		BigFloat(long value)
		{

		}
	
	BigFloat& operator+(BigFloat& obj)
	{
		// 999 999 999
		// 10^9
		// 10^8
		// code that will increase this by obj
		//        10    
		//  12 34 56 78 
		// +
		//  12 46 27 62
		//  ===========
		//           40

		bool overflow = false;
		int base = 1000000000;
		std::vector<std::int32_t> mantissa1 = this->mantissa;
		std::vector<std::int32_t> mantissa2 = obj.mantissa;

		int mantissaLen = std::max(mantissa1.size(), mantissa2.size());
		int m1ptr = mantissa1.size() - 1;
		int m2ptr = mantissa2.size() - 1;

		for (int i = 0; i < mantissaLen; i++) 
		{
			int32_t digit1;
			int32_t digit2;

			if (m1ptr == -1)
				digit1 = 0;
			else
				digit1 = mantissa1[m1ptr];

			if (m2ptr == -1)
				digit2 = 0;
			else
				digit2 = mantissa2[m2ptr];

			int32_t res = digit1 + digit2 + (overflow * base/10);

			overflow = res >= base;
			std::cout << digit1 << " + " << digit2 << " = "<< res << "overflow: " << overflow << std::endl;
			res = res % base;

			(*this).mantissa[m1ptr] = res;

			m1ptr--;
			m2ptr--;
		}
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
};
