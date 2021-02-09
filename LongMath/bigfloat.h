#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"

#define BigNum(x) BigFloat(#x)

class BigFloat
{
	protected:
		bool isNeg = false;
		std::vector<std::int32_t> mantissa;
		int32_t exponent = 0;

	public:
		BigFloat(const char* representation)
		{
			const int power = 9;
			char base1b[power + 2];

			int reprIdx = 0;
			int symbolIdx = 0;
			
			char byte = *(representation);
			isNeg = byte == '-';

			bool countExponent = false;
			bool last = false;
			while (!last)
			{
				last = !*(representation + reprIdx);
				byte = *(representation + reprIdx);
				base1b[symbolIdx] = byte;
				
				exponent += countExponent;
				countExponent = countExponent || byte == '.' || byte == ',';
			
				

				if ( (symbolIdx && !(symbolIdx%power) ) || last)
				{
					base1b[symbolIdx+1] = 0;
					symbolIdx %= power;
					std::cout << parseInt(base1b) << std::endl;
					mantissa.push_back(parseInt(base1b));
				}
				else
				{
					symbolIdx += !(byte == '.' || byte == ',');
				}
				reprIdx++;
			}
		}

		// todo:
		// add numeric constructor
		BigFloat(long value)
		{

		}

	BigFloat* operator+(const BigFloat obj)
	{
		// code that will increase this by obj
		return this;
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
		// code that will print out representation of number
		return os << "";
	}
};
