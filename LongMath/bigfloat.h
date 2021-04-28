#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"
#include <tuple>

#define BigNum(x) BigFloat(#x)
#define basePow 2

struct ParseException : public std::exception
{
	private:
		std::string cause;

	public:
		ParseException() {}

		ParseException(std::string cause) 
		{
			this->cause = cause;
		}

		const std::string getCause() const throw ()
		{
			return cause;
		}
};

class BigFloat
{

protected:

	bool isNeg = false;
	std::vector<std::int32_t> mantissa;
	int32_t exponent = 0;

public:

	// default constructor
	BigFloat() {};

	// string constructor
	BigFloat(const char* repr)
	{
		*this = repr;
	}

	// numeric constructor
	BigFloat(double value)
	{
		*this = std::to_string(value).c_str();
	}

	// copy constructor
	BigFloat(const BigFloat& obj)
	{
		this->exponent = obj.exponent;
		this->mantissa = obj.mantissa;
		this->isNeg = obj.isNeg;
	}
	
	BigFloat(std::vector<int32_t> mantissa) {
		this->mantissa = mantissa;
	}

	BigFloat operator = (const char* repr)
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

	friend BigFloat abs(BigFloat other) {
		other.isNeg = false;
		return other;
	}

	BigFloat normalize()
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

	// arithmetic
	BigFloat& operator-()
	{
		BigFloat copy = BigFloat(*this);
		copy.isNeg = !copy.isNeg;
		return copy.normalize();
	}

	BigFloat& operator+(BigFloat& obj)
	{
		if (this->isNeg && !obj.isNeg)
		{
			obj - (-*(this));
			return this->normalize();
		}
		else if (!this->isNeg && obj.isNeg)
		{
			*(this) - -obj;
			return this->normalize();
		}
	
		bool overflow = false;
		int base = pow(10, basePow);

		std::vector<std::int32_t> mantissa1 = this->mantissa;
		std::vector<std::int32_t> mantissa2 = obj.mantissa;

		int m1ptr = mantissa1.size()-1;
		int m2ptr = mantissa2.size()-1;
		// 123.321
		// 0.123321 * 10 ^ 3 = 123.321

		//   123.321
		//0000.123321
		//           
		// 123.321
		// 1.3
		//
		// 123321
		// 13

		int commaDiff = ((mantissa1.size() - this->exponent) - (mantissa2.size() - obj.exponent));

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

			if (m2ptr < 0|| m2ptr >= mantissa2.size())
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

		this->mantissa = newMantissa;
		return this->normalize();
	}

	BigFloat& operator-(BigFloat& obj)
	{
		bool invert = 0;
		int base = pow(10, basePow);

		auto getDigit = [](int mptr, std::vector<int32_t> mantissa) 
		{ 
			return (mptr >= 0 && mptr < mantissa.size()) ? mantissa[mptr] : 0;  
		};

		// -a -b
		if (this->isNeg && !obj.isNeg)
		{
			obj + (-*(this));
		}

		// a - (-b)
		else if (!this->isNeg && obj.isNeg)
		{
			*(this) + -obj;
		}

		BigFloat top;
		BigFloat bottom;

		if (*this > obj)
		{
			top = *this;
			bottom = obj;
		}
		else
		{
			top = obj;
			bottom = *this;
			invert = true;
		}

		std::vector<std::int32_t> mantissa1 = top.mantissa;
		std::vector<std::int32_t> mantissa2 = bottom.mantissa;

		int32_t exp1 = top.exponent;
		int32_t exp2 = bottom.exponent;

		int m1ptr = mantissa1.size()-1;
		int m2ptr = mantissa2.size()-1;

		int commaDiff = ( (mantissa1.size() - exp1) - (mantissa2.size() - exp2));

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
				digit1 += pow(10, basePow);
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

	BigFloat operator*(const BigFloat obj)
	{
		int exp = exponent + obj.exponent;

		auto len = this->mantissa.size();
		vector<int> res(2 * len);

		if (len == 1) {
			// make elementary multiplication here
			return *this * obj;
		}

		auto k = len / 2;

		vector<int32_t> Xr{ this->mantissa.begin(), this->mantissa.begin() + k };
		vector<int32_t> Xl{ this->mantissa.begin() + k, this->mantissa.end() };
		vector<int32_t> Yr{ obj.mantissa.begin(), obj.mantissa.begin() + k };
		vector<int32_t> Yl{ obj.mantissa.begin() + k, obj.mantissa.end() };

		BigFloat Xln(Xl);
		BigFloat Xrn(Xr);
		BigFloat Yln(Yl);
		BigFloat Yrn(Yr);

		count_++; // was ist das?
		BigFloat P1 = Xln * Yln;
		count_--;
		count_++;
		BigFloat P2 = Xrn * Yrn;
		count_--;
		vector<int32_t> Xlr(k);
		vector<int32_t> Ylr(k);

		for (int i = 0; i < k; ++i) {
			Xlr[i] = Xln.mantissa[i] + Xrn.mantissa[i];
			Ylr[i] = Yln.mantissa[i] + Yrn.mantissa[i];
		}

		BigFloat Xlrn(Xlr); BigFloat Ylrn(Ylr);
		count_++;
		BigFloat P3 = Xlrn * Ylrn;
		count_--;

		for (auto i = 0; i < len; ++i) {
			P3.mantissa[i] -= P2.mantissa[i] + P1.mantissa[i];
		}

		for (auto i = 0; i < len; ++i) {
			res[i] = P2.mantissa[i];
		}

		for (auto i = len; i < 2 * len; ++i) {
			res[i] = P1.mantissa[i - len];
		}

		for (auto i = k; i < len + k; ++i) {
			res[i] += P3.mantissa[i - k];
		}

		BigFloat resn(res);
		if (count_ == 0)
			resn.exponent = exp;

		return resn.normalize();
	}

	BigFloat inverse() const {
		return BigFloat(1) / *this;
	}

	std::tuple<BigFloat, BigFloat> divide(BigFloat other) {

		BigFloat remainder;
		BigFloat result;
		// division logic (long division!!!!!!)
		return std::tuple<BigFloat, BigFloat>{ result, remainder };
	}

	BigFloat operator/(const BigFloat obj)
	{
		BigFloat result, remainder;
		std::tie(result, remainder) = divide(obj);
		return result;
	}

	BigFloat operator%(const BigFloat obj)
	{
		BigFloat result, remainder;
		std::tie(result, remainder) = divide(obj);
		return remainder;
	}

	// comparison
	bool operator==(BigFloat& a)
	{
		if (this->isNeg != a.isNeg)
			return false;

		if (this->exponent != a.exponent)
			return false;

		if (this->mantissa != a.mantissa) 
		{
			BigFloat a1 = *this;
			BigFloat a2 = a;
			a1.normalize();
			a2.normalize();

			BigFloat diff = a1 - a2;
			diff = abs(diff);
			BigFloat maxDiff = BigNum(0.000001);
			return diff.normalize() < maxDiff.normalize();
		}

		return true;
	}

	bool operator<(const BigFloat& obj) 
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

	bool operator!=(BigFloat& obj) 
	{
		return !(*this == obj);
	}

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

	bool operator<=(BigFloat& obj) 
	{
		return !(*this > obj);
	}

	bool operator>=(BigFloat& obj)
	{
		return !(*this < obj);
	}

	// IO
	friend std::ostream& operator<<(std::ostream& os, BigFloat& obj)
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

	friend std::istream& operator>>(std::istream& out, BigFloat& obj)
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
