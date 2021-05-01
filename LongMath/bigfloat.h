#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "ParseException.h"
#include <tuple>
#include <ranges>
#include <algorithm>

#define BigNum(x) BigFloat(#x)
#define BASE_POW 2

const int32_t BASE = pow(10, BASE_POW);

/// <summary>
/// Type for storing big numbers with floating point.
/// </summary>
class BigFloat
{

protected:

	bool m_is_negative = false;
	std::vector<std::int32_t> m_mantissa;
	int32_t m_exponent = 0;

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
	/// Numeric constructor
	/// </summary>
	/// <param name="value">value of number</param>
	BigFloat(int32_t value)
	{
		
	}

	BigFloat(std::vector<int32_t> mantissa)
	{
		m_mantissa = mantissa;
	}

	/// <summary>
	/// Number constructor that allows full control over number fields.
	/// </summary>
	/// <param name="mantissa">mantissa</param>
	/// <param name="exponent">exponent</param>
	/// <param name="neg">negative flag</param>
	BigFloat(std::vector<int32_t> mantissa, int32_t exponent, bool neg) {
		m_is_negative = neg;
		this->m_exponent = exponent;
		this->m_mantissa = mantissa;
	}

	/// <summary>
	/// Copy constructor
	/// </summary>
	/// <param name="x">value to be copied</param>
	BigFloat(const BigFloat& x) = default;

	/// <summary>
	/// Move constructor
	/// </summary>
	/// <param name="x">value to be moved</param>


	/// <summary>
	/// assignment operator. 
	/// Parses string and assignes it to BigFloat instance
	/// </summary>
	/// <param name="repr">string representation of number</param>
	/// <returns>this instance</returns>
	BigFloat& operator = (const char* repr)
	{
		// normalize string
		std::string str = preproc_str(repr);
		if (str.at(0) == '-') 
		{
			m_is_negative = true;
		}

		int digit_count = str.size();
		int dot_idx = str.find('.');

		if (dot_idx != -1)
		{
			digit_count = dot_idx;
			str = str.erase(dot_idx, 1);
		}

		int cur_ptr = 0;
		int str_idx = digit_count % BASE_POW;

		std::string digit = std::string(BASE_POW, '0');

		while (cur_ptr < str.size())
		{
			digit[str_idx] = str[cur_ptr];

			if (str_idx == BASE_POW - 1)
			{
				this->m_mantissa.push_back(parseInt(digit));
				digit = std::string(BASE_POW, '0');
			}

			cur_ptr++;
			str_idx++;
			str_idx %= BASE_POW;
		}

		// just in case
		m_mantissa.push_back(parseInt(digit));

		// count exponent based on dot position
		m_exponent = ceil(digit_count / (float)BASE_POW);
		
		return normalize();
	}

	BigFloat& operator = (BigFloat const& x) = default;
	BigFloat& operator = (BigFloat&& x) = default;

	/// <summary>
	/// Validate and normalize BigFloat representation
	/// </summary>
	/// <param name="s">input string</param>
	/// <returns>output string</returns>
	std::string preproc_str(const char* s)
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
	friend BigFloat abs(BigFloat y)
	{
		y.m_is_negative = false;
		return y;
	}

	/// <summary>
	/// Normalization function that removes leading zeroes
	/// </summary>
	/// <returns>updated instance</returns>
	BigFloat& normalize()
	{
		// remove leading zeros and move them to exponent
		auto n = std::find_if(m_mantissa.begin(), m_mantissa.end(), [this](auto x) { return x != 0; });
		m_exponent -= n - m_mantissa.begin();
		m_mantissa.erase(m_mantissa.begin(), n);
		

		while (!m_mantissa.empty() && !m_mantissa.back()) m_mantissa.pop_back();

		// we don't want -0
		if ( zero() ) {
			m_is_negative = false;
		}

		return *this;
	}

	/// <summary>
	///	cast BigFloat to double
	/// </summary>
	/// <returns>double representation of BigFloat</returns>
	double to_double()
	{
		if (m_exponent > 127)
		{
			return INFINITY;
		}

		if (m_exponent < -127)
		{
			return 0;
		}
		double d = parseDouble(m_mantissa, m_exponent, BASE);
		return d;
	}

	
	/// <summary>
	/// Unary minus. invert isNeg flag of BigFloat
	/// </summary>
	/// <returns>new inverted instance</returns>
	friend BigFloat operator-(BigFloat y)
	{
		y.m_is_negative = !y.m_is_negative;
		return y;
	}

	/// <summary>
	/// Adds two BigFloat numbers together
	/// </summary>
	/// <param name="obj">other BigFloat</param>
	/// <returns>new instance with value of this object increased by obj</returns>
	friend BigFloat operator+(BigFloat const& x, BigFloat const& y)
	{
		BigFloat result;

		if (x.m_is_negative && !y.m_is_negative)
		{
			return y - -x;
		}
		else if (!x.m_is_negative && y.m_is_negative)
		{
			return x - -y;
		}
	
		auto get_with_rank = [](BigFloat const& x, int i) {
			int idx = x.m_exponent - 1 - i;
			return idx >= x.m_mantissa.size() ? 0 : x.m_mantissa.at(idx);
		};

		int lb = std::max((x.m_mantissa.size() - x.m_exponent), (y.m_mantissa.size() - y.m_exponent));
		lb = lb < 0 ? 0 : -lb;
		int max_exp = std::max(x.m_exponent, y.m_exponent);
		result.m_exponent = max_exp;
		
		bool overflow = false;

		while (lb < max_exp)
		{
			int32_t x_digit = get_with_rank(x, lb);
			int32_t y_digit = get_with_rank(y, lb);


			int32_t res = x_digit + y_digit + overflow;

			overflow = res >= BASE;
			res = res % BASE;

			result.m_mantissa.push_back(res);
			lb++;
		}
		if (overflow) {
			result.m_mantissa.push_back(overflow);
			result.m_exponent++;
		}
		std::reverse(result.m_mantissa.begin(), result.m_mantissa.end());
	
		return result.normalize();
	}

	/// <summary>
	/// Substracts one BigFloat value from other
	/// </summary>
	/// <param name="obj">BigFloat to substract from this instance</param>
	/// <returns>new instance with value of this object decreased by obj</returns>
	friend BigFloat operator-(BigFloat x, BigFloat y)
	{
		BigFloat result;

		if (x.m_is_negative && !y.m_is_negative)
		{
			return y - -x;
		}
		else if (!x.m_is_negative && y.m_is_negative)
		{
			return x - -y;
		}
		bool invert = false;

		if (y > x) {
			invert = true;
			BigFloat b = x;
			x = y;
			y = b;
		}

		auto get_relative = [](BigFloat const& x, int i) {
			int dot = x.m_mantissa.size() - x.m_exponent;
			int idx = x.m_mantissa.size() - (dot + i) - 1;
			return x.m_mantissa.size() <= idx ? 0 : x.m_mantissa.at(idx);
		};

		int lb = std::max((x.m_mantissa.size() - x.m_exponent), (y.m_mantissa.size() - y.m_exponent));
		lb = lb < 0 ? 0 : -lb;
		int max_exp = std::max(x.m_exponent, y.m_exponent);
		result.m_exponent = max_exp;

		bool prev_borrow = false;
		while (lb < max_exp)
		{
			bool borrow = false;
			

			int32_t x_digit = get_relative(x, lb);
			int32_t y_digit = get_relative(y, lb);

			if (x_digit-prev_borrow < y_digit) {
				x_digit += BASE;
				borrow = true;
			}

			int32_t res = (x_digit-prev_borrow) - y_digit;
			result.m_mantissa.push_back(res);
			lb++;
			prev_borrow = borrow;
		}

		std::reverse(result.m_mantissa.begin(), result.m_mantissa.end());

		if (invert)
			result = !result;

		return result.normalize();
	}

	BigFloat floor() {
		return *this;
	}
	
	/// <summary>
	/// Operator that multiplies BigFloat by digit. Inplements long multiplication algorithim since its effective enough here. 
	/// </summary>
	/// <param name="x">BigFloat</param>
	/// <param name="y">int64_t, digit</param>
	/// <returns></returns>
	friend BigFloat operator*(const BigFloat x, const int32_t y) {

		if (y >= BASE) {
			throw std::string(y + " is too big!");
		}

		BigFloat result;
		result.m_exponent = x.m_exponent;
		int32_t overflow = 0;

		for (auto i = x.m_mantissa.rbegin(); i != x.m_mantissa.rend(); i++) {

			int64_t digit = *i;

			int64_t composition = digit * y;
			composition += overflow;

			int32_t remainder = composition % BASE;
			overflow = (composition - remainder) / BASE;
			result.m_mantissa.push_back(remainder);
		}

		if (overflow) {
			result.m_mantissa.push_back(overflow);
			result.m_exponent += 1;
		}

		std::reverse(result.m_mantissa.begin(), result.m_mantissa.end());

		return result.normalize();
	}

	/// <summary>
	/// Operator that multiplies BigFloat by BigFloat. Implements Caratsuba's algorithm.
	/// </summary>
	/// <param name="x">BigFloat</param>
	/// <param name="y">BigFloat</param>
	/// <returns></returns>
	friend BigFloat operator*(BigFloat x, BigFloat y)
	{
		int x_exp_offset = x.m_mantissa.size() - x.m_exponent;
		int y_exp_offset = y.m_mantissa.size() - y.m_exponent;

		x.m_exponent += x_exp_offset;
		y.m_exponent += y_exp_offset;

		int32_t exp = x.m_exponent + y.m_exponent;
		bool sign = false;

		auto get_sign = [](BigFloat const& x, BigFloat const& y) {
			return x.m_is_negative ^ y.m_is_negative;
		};

		auto x_len = x.m_mantissa.size();
		auto y_len = y.m_mantissa.size();

		if (x_len == 1 || y_len == 1) {
			// elementary multiplication 
			BigFloat res = ( (x_len == 1) ? y * (x.m_mantissa.at(0)) : x * y.m_mantissa.at(0));
			res.m_is_negative = get_sign(x, y);
			return res;
		}

		int x_k = std::ceil(x_len / 2.);
		int y_k = std::ceil(y_len / 2.);

		std::vector<int32_t>vec_a({ x.m_mantissa.begin(),       x.m_mantissa.begin() + x_k });
		std::vector<int32_t>vec_b({ x.m_mantissa.begin() + x_k, x.m_mantissa.end()         });
		std::vector<int32_t>vec_c({ y.m_mantissa.begin(),       y.m_mantissa.begin() + y_k });
		std::vector<int32_t>vec_d({ y.m_mantissa.begin() + y_k, y.m_mantissa.end()         });

		BigFloat a(vec_a, x_k,         false);
		BigFloat b(vec_b, x_len - x_k, false);
		BigFloat c(vec_c, y_k,         false);
		BigFloat d(vec_d, y_len - y_k, false);

		BigFloat ac = a * c;
		BigFloat bd = b * d;
		BigFloat sum1 = c + d;
		BigFloat sum2 = a + b;
		BigFloat sum3 = ac + bd;
		BigFloat mul1 = sum1 * sum2;
		BigFloat dif1 = mul1 - sum3;
		BigFloat mid = (c + d) * (a + b) - (ac + bd);

		ac.m_exponent += 2;
		mid.m_exponent += 1;
		
		// todo chaining?
		BigFloat resn;

		resn = ac + mid + bd;

		resn.m_exponent -= (x_exp_offset + y_exp_offset);
		resn.m_is_negative = sign;

		return resn.normalize();
	}

	BigFloat inverse() const {
		//picarte's iteration here
		return 0;
	}

	/// <summary>
	/// Divide BigFloat by BigFloat. Effectively multiplies by inverse number
	/// </summary>
	/// <param name="x">BigFloat dividend</param>
	/// <param name="y">BigFloat divisor</param>
	/// <returns>quotient</returns>
	friend BigFloat operator/(const BigFloat x, const BigFloat y)
	{
		return x * y.inverse();
	}
	
	bool zero() const {
		return m_mantissa.empty();
	}

	/// <summary>
	/// Checks equality of two BigFloat objects
	/// </summary>
	/// <param name="obj">other object</param>
	/// <returns>whether represented values differ less then maximal allowed amount</returns>
	friend bool operator==(BigFloat const& x, BigFloat const& y) = default;

	std::strong_ordering operator <=>(BigFloat const& other) const
	{
		if (auto c = other.m_is_negative <=> m_is_negative; c != 0)
		{
			return c;
		}

		auto maybe_invert = [this](auto c) { return m_is_negative ? 0 <=> c : c; };

		if (auto c = m_exponent <=> other.m_exponent; c != 0)
		{
			return maybe_invert(c);
		}
		
		return maybe_invert(std::lexicographical_compare_three_way(m_mantissa.begin(), m_mantissa.end(), other.m_mantissa.begin(), other.m_mantissa.end()));
	}

	/// <summary>
	/// Outputs Bigfloat into Stream
	/// </summary>
	/// <param name="os">stream instance to output into</param>
	/// <param name="obj">instance to output</param>
	/// <returns>same stream instance</returns>
	friend std::ostream& operator<<(std::ostream& os, BigFloat const& y)
	{
		std::vector<int32_t> digits = y.m_mantissa;

		int digCount = digits.size();

		std::string repr = y.m_is_negative ? "-0." : "+0.";
		
		for (int32_t dig : y.m_mantissa)
		{
			if (dig < 0)
				dig = -dig;

			std::string digit = parseString(dig, BASE_POW);
			repr += digit;
		}
		repr += "(B)";
		repr += parseString(y.m_exponent);
		return os << repr;
	}

	/// <summary>
	/// Initializes Bigfloat from Stream
	/// </summary>
	/// <param name="is">stream instance to initialize from</param>
	/// <param name="obj">instance to initialize</param>
	/// <returns>same stream instance</returns>
	friend std::istream& operator>>(std::istream& is, BigFloat& y)
	{	
		std::string repr = std::string();
		char c;
		while (1) 
		{
			is.get(c);

			if (c == 10)
				break;
			repr.push_back(c);
		}
		y = repr.c_str();
	
		return is;
	}

	friend BigFloat operator!(BigFloat x) 
	{
		x.m_is_negative = !x.m_is_negative;
		return x;
	}

	BigFloat operator += (BigFloat const& b) {
		return *this = *this + b;
	}
};

/// <summary>
/// Gets remainder of division of this instance by obj
/// </summary>
/// <param name="obj">BigFloat to divide by this instance</param>
/// <returns>new instance with value of remainder</returns>
BigFloat operator%(BigFloat const& x, BigFloat const& y) { return x - (x / y).floor() * y; }



