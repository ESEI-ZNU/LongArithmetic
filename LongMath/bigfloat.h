#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"
#include "ParseException.h"
#include <tuple>
#include <ranges>

using namespace std::ranges;

#define BigNum(x) BigFloat(#x)
#define basePow 2

/// <summary>
/// Type for storing big numbers with floating point.
/// </summary>
class BigFloat
{

protected:

	bool isNeg = false;
	std::vector<std::int32_t> mantissa;
	int32_t exponent = 0;

public:

	/// <summary>
	/// Default constructor
	/// </summary>
	BigFloat() {};

	/// <summary>
	/// Constructor from string
	/// </summary>
	/// <param name="repr">Number representation</param>
	BigFloat(const char* repr)
	{
		*this = repr;
	}

	/// <summary>
	/// Constructor from double
	/// </summary>
	/// <param name="value">Value to be converted into BigFloat type</param>
	BigFloat(double value)
	{
		*this = std::to_string(value).c_str();
	}

	/// <summary>
	/// Copy constructor
	/// </summary>
	/// <param name="obj">value to be copied</param>
	BigFloat(int32_t value)
	{
		
	}

	BigFloat(std::vector<int32_t> mantissa, int32_t exponent, bool neg) {
		isNeg = neg;
		this->exponent = exponent;
		this->mantissa = mantissa;
	}

	// copy constructor
	BigFloat(const BigFloat& obj)
	{
		this->exponent = obj.exponent;
		this->mantissa = obj.mantissa;
		this->isNeg = obj.isNeg;
	}

	/// <summary>
	/// assignment operator. 
	/// Parses string and assignes it to BigFloat instance
	/// </summary>
	/// <param name="repr">string representation of number</param>
	/// <returns>this instance</returns>
	BigFloat& operator = (const char* repr)
	{
		// normalize string
		std::string str = preprocStr(repr);
		if (str.at(0) == '-') 
		{
			isNeg = true;
		}

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
		
		return this->normalize();
	}

	/// <summary>
	/// Validate and normalize BigFloat representation
	/// </summary>
	/// <param name="s">input string</param>
	/// <returns></returns>
	std::string preprocStr(const char* s)
	{
		std::string str = std::string(s);
		int idx = 0;
		bool prevIsNull = true;
		bool dotPresent = false;
	
		auto isValid = [](char c)
		{
			return ( (c > 47 && c < 58) || c == '.' || c == '_' || c == '-');
		};

		// remove all underscores and leading zeros
		while (idx < str.length())
		{
			char c = str[idx];

			if (c == '_' || c == '\'')
			{
				str.erase(idx, 1);
				continue;
			}

			if (!isValid(c))
				throw ParseException("Invalid character");

			if (c == '-' && idx != 0) 
				throw ParseException("Minus at wrong position");

			if (c == '.' && dotPresent)
				throw ParseException("Duplicated dot");
			
			dotPresent = dotPresent || c == '.';

			if (prevIsNull && c == '0')
			{
				prevIsNull = true;
				str = str.erase(idx, 1);
				continue;
			}

			prevIsNull = prevIsNull && c == '0';

			idx++;
		}
		return str;
	}


	/// <summary>
	/// changes isNeg value of BigFloat copy to true (positive).
	/// </summary>
	/// <param name="obj">BigFloat to get absolute value of</param>
	/// <returns>new positive instance of BigFloat</returns>
	friend BigFloat abs(BigFloat obj)
	{
		obj.isNeg = false;
		return obj;
	}

	/// <summary>
	/// Normalization function that removes leading zeroes
	/// </summary>
	/// <returns>updated instance</returns>
	BigFloat& normalize()
	{
		// remove leading zeros and move them to exponent
		for (int32_t digit : this->mantissa)
		{
			if (digit == 0)
			{
				this->mantissa.erase(mantissa.begin());
				if (exponent <= 0)
				{
					this->exponent--;
				}
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
		return *this;
	}

	/// <summary>
	///	cast BigFloat to double
	/// </summary>
	/// <returns>double representation of BigFloat</returns>
	[[deprecated]]
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
		double d = parseDouble(this->mantissa, this->exponent, pow(10, basePow));
		return d;
	}

	
	/// <summary>
	/// Unary minus. invert isNeg flag of BigFloat
	/// </summary>
	/// <returns>new inverted instance</returns>
	friend BigFloat operator-(BigFloat obj)
	{
		obj.isNeg = !obj.isNeg;
		return obj;
	}

	/// <summary>
	/// Adds two BigFloat numbers together
	/// </summary>
	/// <param name="obj">other BigFloat</param>
	/// <returns>new instance with value of this object increased by obj</returns>
	BigFloat operator+(BigFloat obj)
	friend BigFloat operator+(BigFloat x, BigFloat obj)
	{
		if (x.isNeg && !obj.isNeg)
		{
			obj - (-x);
			return x.normalize();
		}
		else if (!x.isNeg && obj.isNeg)
		{
			x - -obj;
			return x.normalize();
		}
	
		bool overflow = false;
		int base = pow(10, basePow);

		std::vector<std::int32_t> mantissa1 = x.mantissa;
		std::vector<std::int32_t> mantissa2 = obj.mantissa;

		int m1ptr = mantissa1.size() - 1;
		int m2ptr = mantissa2.size() - 1;

		int commaDiff = ((mantissa1.size() - x.exponent) - (mantissa2.size() - obj.exponent));

		// move pointer to match commas
		if (commaDiff > 0)
			m2ptr += commaDiff;
		else
			m1ptr += -commaDiff;

		std::vector<int32_t> newMantissa;

		while (m1ptr < mantissa1.size() || m2ptr < mantissa2.size() || overflow)
		{
			int32_t digit1;
			int32_t digit2;

			if (m1ptr < 0 || m1ptr >= mantissa1.size())
				digit1 = 0;
			else
				digit1 = mantissa1[m1ptr];

			if (m2ptr < 0 || m2ptr >= mantissa2.size())
				digit2 = 0;
			else
				digit2 = mantissa2[m2ptr];

			int32_t res = digit1 + digit2 + overflow;
			//std::cout << digit1 << " + " << digit2 << " = " << res << " add overflow: " << overflow << std::endl;
			overflow = res >= base;

			res = res % base;
			newMantissa.insert(newMantissa.begin(), res);

			m1ptr--;
			m2ptr--;
		}

		x.mantissa = newMantissa;
		return x.normalize();
	}

	/// <summary>
	/// Substracts one BigFloat value from other
	/// </summary>
	/// <param name="obj">BigFloat to substract from this instance</param>
	/// <returns>new instance with value of this object decreased by obj</returns>
	BigFloat operator-(BigFloat obj)
	{
		bool invert = 0;
		int base = pow(10, basePow);

		auto getDigit = [](int mptr, std::vector<int32_t> mantissa)
		{
			return (mptr >= 0 && mptr < mantissa.size()) ? mantissa[mptr] : 0;
		};

		// -a -b
		if (x.isNeg && !obj.isNeg)
		{
			return obj + -x;
		}

		// a - (-b)
		else if (!x.isNeg && obj.isNeg)
		{
			return x + -obj;
		}

		BigFloat top;
		BigFloat bottom;

		if (x > obj)
		{
			top = x;
			bottom = obj;
		}
		else
		{
			top = obj;
			bottom = x;
			invert = true;
		}

		std::vector<std::int32_t> mantissa1 = top.mantissa;
		std::vector<std::int32_t> mantissa2 = bottom.mantissa;

		int32_t exp1 = top.exponent;
		int32_t exp2 = bottom.exponent;

		int m1ptr = mantissa1.size() - 1;
		int m2ptr = mantissa2.size() - 1;

		int commaDiff = ((mantissa1.size() - exp1) - (mantissa2.size() - exp2));

		// move pointer to match commas
		// first number has comma commaDiff positions further to right

		if (commaDiff > 0)
			m2ptr += commaDiff;
		else
			m1ptr += -commaDiff;

		std::vector<int32_t> newMantissa;

		bool borrow = false;

		while (m1ptr < mantissa1.size() || m2ptr < mantissa2.size())
		{
			int32_t digit1 = getDigit(m1ptr, mantissa1);
			int32_t digit2 = getDigit(m2ptr, mantissa2);

			if (borrow)
			{
				digit1--;
				borrow = false;
			}

			if (digit1 < digit2)
			{
				borrow = true;
				digit1 += BASE;
			}

			int32_t res = digit1 - digit2;
			newMantissa.insert(newMantissa.begin(), res);

			m1ptr--;
			m2ptr--;
		}

		this->mantissa = newMantissa;
		if (invert)
			this->isNeg = !this->isNeg;

		return this->normalize();
	}
	
	friend BigFloat operator*(const BigFloat x, const int64_t y) {
		
		BigFloat result;
		int32_t overflow = 0;

		for (int64_t digit : x.mantissa | views::reverse) {
			int64_t composition = digit * y;
			composition += overflow;

			int32_t remainder = composition % BASE;
			overflow = (composition - remainder) / BASE;
			result.mantissa.push_back(remainder);
		}

		return result;
	}

	friend BigFloat operator*(const BigFloat x, const BigFloat y)
	{
		// ac * bd = ac     (c+d)*(a+b)-(ac+bd)    bd
		
		int32_t exp = exponent + obj.exponent;
		bool sign = false;

		auto x_len = x.mantissa.size();
		auto y_len = y.mantissa.size();

		if (x_len == 1 || y_len == 1) {
			// elementary multiplication 
			BigFloat res = ( (x_len == 1) ? y * (x.mantissa.at(0)) : x * y.mantissa.at(0));
			res.exponent = exp;
			res.isNeg = sign;
			return res;
		}

		auto x_k = x_len / 2;
		auto y_k = y_len / 2;

		BigFloat a({ x.mantissa.begin(),       x.mantissa.begin() + x_k });
		BigFloat b({ x.mantissa.begin() + x_k, x.mantissa.end()         });
		BigFloat c({ y.mantissa.begin(),         y.mantissa.begin() + y_k   });
		BigFloat d({ y.mantissa.begin() + y_k,   y.mantissa.end()           });

		BigFloat ac = a * c;
		BigFloat bd = b * d;
		BigFloat mid = (c + d) * (a + b) - (ac + bd);

		ac.exponent += 2;
		mid.exponent += 1;
		
		BigFloat resn;

		resn += ac;
		resn += mid;
		resn += bd;

		resn.exponent = exp;
		resn.isNeg = sign;

		return resn.normalize();
	}

	BigFloat inverse() const {
		//picarte's iteration here
		return 0;
	}

	/// <summary>
	/// Gets remainder of division of this instance by obj
	/// </summary>
	/// <param name="obj">BigFloat to divide by this instance</param>
	/// <returns>new instance with value of remainder</returns>
	BigFloat operator%(const BigFloat obj)
	{
		return x * obj.inverse();
	}

	/// <summary>
	/// Checks equality of two BigFloat objects
	/// </summary>
	/// <param name="obj">other object</param>
	/// <returns>whether represented values differ less then maximal allowed amount</returns>
	bool operator==(BigFloat& obj)
	{
		if (this->isNeg != obj.isNeg)
			return false;

		if (this->exponent != obj.exponent)
			return false;

		if (this->mantissa != obj.mantissa)
		{
			BigFloat a1 = *this;
			BigFloat a2 = obj;
			a1.normalize();
			a2.normalize();

			BigFloat diff = a1 - a2;
			diff = abs(diff);
			BigFloat maxDiff = BigNum(0.000001);
			return diff.normalize() < maxDiff.normalize();
		}

		return true;
	}

	/// <summary>
	/// Checks if left side BigFloat is less then right side
	/// </summary>
	/// <param name="obj">right side BigFloat</param>
	/// <returns>whether left value is less then right value</returns>
	bool operator<(BigFloat& obj) 
	{
		// different sign
		if (this->isNeg != obj.isNeg) 
		{
			return this->isNeg;
		}

		// same exponent
		if (this->exponent == obj.exponent)
		{
			return this->mantissa < obj.mantissa;
		}

		// both negative
		if (this->isNeg) 
		{
			return this->exponent > obj.exponent;
		}
		// both positive
		else 
		{
			return this->exponent < obj.exponent;
		}
	}

	/// <summary>
	/// Checks if two BigFloats are not equal
	/// </summary>
	/// <param name="obj">other BigFloat</param>
	/// <returns>whether two values are not equal</returns>
	bool operator!=(BigFloat& obj) 
	{
		return !(*this == obj);
	}

	/// <summary>
	/// Checks if left side BigFloat is greater then right side
	/// </summary>
	/// <param name="obj">right side BigFloat</param>
	/// <returns>whether left value is greater then right value</returns>
	bool operator>(BigFloat& obj) 
	{
		// different sign
		if (this->isNeg != obj.isNeg)
		{
			return !this->isNeg;
		}

		// same exponent
		if (this->exponent == obj.exponent)
		{
			return this->mantissa > obj.mantissa;
		}

		// both negative
		if (this->isNeg)
		{
			return this->exponent < obj.exponent;
		}
		// both positive
		else
		{
			return this->exponent > obj.exponent;
		}
	}
	
	/// <summary>
	/// Checks if left side BigFloat is less or equal to the right side
	/// </summary>
	/// <param name="obj">right side BigFloat</param>
	/// <returns>whether left value is less or equal to the right value</returns>
	bool operator<=(BigFloat& obj) 
	{
		return !(*this > obj);
	}

	/// <summary>
	/// Checks if left side BigFloat is greater or equal to the right side
	/// </summary>
	/// <param name="obj">right side BigFloat</param>
	/// <returns>whether left value is greater or equal to the right value</returns>
	bool operator>=(BigFloat& obj)
	{
		return !(*this < obj);
	}

	/// <summary>
	/// Outputs Bigfloat into Stream
	/// </summary>
	/// <param name="os">stream instance to output into</param>
	/// <param name="obj">instance to output</param>
	/// <returns>same stream instance</returns>
	friend std::ostream& operator<<(std::ostream& os, BigFloat obj)
	{
		std::vector<int32_t> digits = obj.mantissa;

		int digCount = digits.size();

		std::string repr = obj.isNeg ? "-0." : "+0.";
		
		for (int32_t dig : obj.mantissa)
		{
			if (dig < 0)
				dig = -dig;

			std::string digit = std::string(parseString(dig, basePow));
			repr += digit;
		}
		repr += "(B)";
		repr += parseString(obj.exponent);
		return os << repr;
	}

	/// <summary>
	/// Initializes Bigfloat from Stream
	/// </summary>
	/// <param name="is">stream instance to initialize from</param>
	/// <param name="obj">instance to initialize</param>
	/// <returns>same stream instance</returns>
	friend std::istream& operator>>(std::istream& is, BigFloat& obj)
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
