#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <tuple>
#include <ranges>
#include <algorithm>
#include "conversions.h"


/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// <para>Type for storing big numbers with floating point.</para>
/// </summary>
class BigFloat
{

private:

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>is_negative setter</para>
	/// </summary>
	/// <param name="is_negative">flag value</param>
	/// <returns>updated instance</returns>
	BigFloat& set_negative(bool is_negative) {
		m_is_negative = is_negative;
		return *this;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>exponent setter</para>
	/// </summary>
	/// <param name="delta">exponent difference</param>
	/// <returns>updated instance</returns>
	BigFloat& add_exp(int32_t delta) {
		m_exponent += delta;
		return *this;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>exponent setter</para>
	/// </summary>
	/// <param name="exp">new exponent</param>
	/// <returns>updated instance</returns>
	BigFloat& set_exp(int32_t exp) {
		m_exponent = exp;
		return *this;
	}

	bool m_is_negative = false;
	std::vector<std::int32_t> m_mantissa;
	int32_t m_exponent = 0;

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Getter for digits that can default to value if one does not exist in vector</para>
	/// </summary>
	/// <param name="index">index to get value at</param>
	/// <param name="default_value">value to return if digit not present</param>
	/// <returns>digit of number or default value if not present</returns>
	int32_t defaulting_digit(int32_t index, int32_t default_value) const {
		if (index < 0 || index >= m_mantissa.size())
			return default_value;
		return m_mantissa.at(index);
	}

public:

	static const int32_t MAX_EXP = INT32_MAX;
	static const int32_t MIN_EXP = INT32_MIN;

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Default constructor</para>
	/// </summary>
	BigFloat() {};

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Constructor from string</para>
	/// </summary>
	/// <param name="repr">Number representation</param>
	BigFloat(const char* repr)
	{
		*this = repr;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Constructor from std::string</para>
	/// </summary>
	/// <param name="repr">string representation of number</param>
	BigFloat(const std::string repr)
	{
		*this = repr.c_str();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Constructor from double</para>
	/// </summary>
	/// <param name="value">Value to be converted into BigFloat type</param>
	BigFloat(double value)
	{
		*this = std::to_string(value).c_str();
	}


	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>constructor from single digit</para>
	/// </summary>
	/// <param name="value">value of number</param>
	BigFloat(int32_t value)
	{
		this->m_mantissa = std::vector<int32_t>{ value };
		this->m_exponent = 1;
		normalize();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Constructior from mantissa. leaves other fields as they are</para>
	/// </summary>
	/// <param name="mantissa">mantissa of the number</param>
	BigFloat(std::vector<int32_t> mantissa)
	{
		m_mantissa = mantissa;
		normalize();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Number constructor that allows full control over number fields.</para>
	/// </summary>
	/// <param name="mantissa">mantissa of the number</param>
	/// <param name="exponent">exponent of the number</param>
	/// <param name="neg">negative flag</param>
	BigFloat(std::vector<int32_t> mantissa, int32_t exponent, bool neg) {
		m_is_negative = neg;
		this->m_exponent = exponent;
		this->m_mantissa = mantissa;
		normalize();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Number constructor that allows full control over number fields.</para>
	/// </summary>
	/// <param name="initializer">field values (is negative, mantissa, exponent) packed into tuple.</param>
	BigFloat(std::tuple<bool, std::vector<int32_t>, int32_t> initializer) {
		std::tie(m_is_negative, m_mantissa, m_exponent) = initializer;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Copy constructor</para>
	/// </summary>
	/// <param name="x">value to be copied</param>
	BigFloat(const BigFloat& x) = default;


	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>assignment operator. </para>
	/// <para>Parses string and assignes it to BigFloat instance</para>
	/// </summary>
	/// <param name="repr">string representation of number</param>
	/// <returns>this instance</returns>
	BigFloat& operator = (const char* repr)
	{
		std::tie(m_is_negative, m_mantissa, m_exponent) = parse_bigfloat(normalize_repr(repr));
		return normalize();
	}

	BigFloat& operator = (BigFloat const& x) = default;
	BigFloat& operator = (BigFloat&& x) = default;

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>changes isNeg value of BigFloat copy to true (positive).</para>
	/// </summary>
	/// <param name="obj">BigFloat to get absolute value of</param>
	/// <returns>new positive instance of BigFloat</returns>
	friend BigFloat abs(BigFloat y) {
		return y.set_negative(false);
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Normalization function that removes leading zeroes</para>
	/// </summary>
	/// <returns>updated instance</returns>
	BigFloat& normalize()
	{
		// remove leading zeros and move them to exponent
		auto n = std::find_if(m_mantissa.begin(), m_mantissa.end(), [this](auto x) { return x != 0; });
		m_exponent -= n - m_mantissa.begin();
		m_mantissa.erase(m_mantissa.begin(), n);

		while (!m_mantissa.empty() && !m_mantissa.back()) m_mantissa.pop_back();

		if ( zero() ) {
			m_is_negative = false;
			m_exponent = MIN_EXP;
		}

		return *this;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	///	<para>cast BigFloat to double</para>
	/// </summary>
	/// <returns>double representation of BigFloat</returns>
	double to_double() const
	{
		if (m_exponent > 127)
			return m_is_negative ? -1 : 1 * INFINITY;

		if (m_exponent < -127)
			return 0;
		
		return parseDouble(m_mantissa, m_exponent, BASE);
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>converts nmumber to it's string representation</para>
	/// </summary>
	/// <returns>number as string</returns>
	std::string to_string() const {

		if (zero()) {
			return "0";
		}

		std::string repr = m_is_negative ? "-" : "";
		
		int digit_count = 0;
		for (int32_t dig : m_mantissa)
		{
			std::string digit = std::string(count_digits(dig) % BASE_POW, '0') + std::to_string(dig);
			if (digit_count == m_exponent)  {
				repr += '.';
			}
			repr += digit;
			digit_count++;
		}

		return repr;
	}

	
	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Unary minus. invert isNeg flag of BigFloat</para>
	/// </summary>
	/// <returns>new inverted instance</returns>
	friend BigFloat operator-(BigFloat y)
	{
		return y.set_negative(!y.m_is_negative);
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Adds two BigFloat numbers together</para>
	/// </summary>
	/// <param name="obj">other BigFloat</param>
	/// <returns>new instance with value of this object increased by obj</returns>
	friend BigFloat operator+(BigFloat const& x, BigFloat const& y)
	{
		if (x.zero()) {
			return y;
		}

		if (y.zero()) {
			return x;
		}

		if (x.m_is_negative) {
			return y - abs(x);
		}

		else if (y.m_is_negative) {
			return x - abs(y);
		}

		BigFloat result;

		int32_t out_length = std::max(x.m_mantissa.size(), y.m_mantissa.size()) + abs(x.m_exponent - y.m_exponent);

		auto get_aligned = [](int i, BigFloat const& x, BigFloat const& other) {
			int exp_offset = std::max(0, other.m_exponent - x.m_exponent);
			return x.defaulting_digit(i - exp_offset, 0);
		};
		
		int lb = out_length-1;
		int max_exp = std::max(x.m_exponent, y.m_exponent);
		result.m_exponent = max_exp;
		
		bool overflow = false;

		while (lb >= 0)
		{
			int32_t x_digit = get_aligned(lb, x, y);
			int32_t y_digit = get_aligned(lb, y, x);

			int32_t res = x_digit + y_digit + overflow;

			overflow = res >= BASE;
			res = res % BASE;

			result.m_mantissa.push_back(res);
			lb--;
		}
		if (overflow) {
			result.m_mantissa.push_back(overflow);
			result.m_exponent++;
		}
		std::reverse(result.m_mantissa.begin(), result.m_mantissa.end());
	
		return result.normalize();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Substracts one BigFloat value from other</para>
	/// </summary>
	/// <param name="obj">BigFloat to substract from this instance</param>
	/// <returns>new instance with value of this object decreased by obj</returns>
	friend BigFloat operator-(BigFloat x, BigFloat y)
	{
		if (x.zero()) {
			return -y;
		}
		if (y.zero()) {
			return x;
		}

		if (x.m_is_negative) {
			return - (abs(x) + y);
		}

		else if (y.m_is_negative) {
			return x + abs(y);
		}
		
		BigFloat result;

		if (y > x) {
			result.m_is_negative = true;
			BigFloat b = x;
			x = y;
			y = b;
		}

		int32_t out_length = std::max(x.m_mantissa.size(), y.m_mantissa.size()) + abs(x.m_exponent - y.m_exponent);

		auto get_aligned = [](int i, BigFloat const& x, BigFloat const& other) {

			int exp_offset = std::max(0, other.m_exponent - x.m_exponent);
			return x.defaulting_digit(i - exp_offset, 0);
		};

		int lb = out_length - 1;
		int max_exp = std::max(x.m_exponent, y.m_exponent);
		result.m_exponent = max_exp;

		bool prev_borrow = false;
		while (lb >= 0)
		{
			bool borrow = false;
			
			int32_t x_digit = get_aligned(lb, x, y);
			int32_t y_digit = get_aligned(lb, y, x);

			if (x_digit-prev_borrow < y_digit) {
				x_digit += BASE;
				borrow = true;
			}

			int32_t res = (x_digit-prev_borrow) - y_digit;
			result.m_mantissa.push_back(res);
			lb--;
			prev_borrow = borrow;
		}

		std::reverse(result.m_mantissa.begin(), result.m_mantissa.end());

		return result.normalize();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Round the bigfloat downwards removing fractial part</para>
	/// </summary>
	/// <returns>whole bigfloat number copy</returns>
	BigFloat floor() {
		BigFloat res = *this;
		res.m_mantissa.erase(res.m_mantissa.begin() + res.m_exponent, res.m_mantissa.end());
		return res;
	}
	
	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Operator that multiplies BigFloat by digit. Inplements long multiplication algorithim since its effective enough here. </para>
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


	friend BigFloat mul_fast_0exp(BigFloat const& x, BigFloat const& y) {

		if (x.zero() || y.zero()) {
			return 0;
		}

		auto x_len = x.m_mantissa.size();
		auto y_len = y.m_mantissa.size();


		if (x_len == 1 && y_len == 1) {
			// elementary multiplication 
			int64_t comp = (int64_t) x.m_mantissa.at(0) * y.m_mantissa.at(0);
			int32_t remainder = comp % BASE;
			BigFloat s = (int32_t)(comp - remainder) / BASE;
			BigFloat m = remainder;
			s.m_exponent += 1;
			return s + m;
		}
		
		int k = std::max(std::ceil(x_len / 2.), std::ceil(y_len / 2.));

		std::vector<int32_t>vec_a(k);
		std::vector<int32_t>vec_b(k);
		std::vector<int32_t>vec_c(k);
		std::vector<int32_t>vec_d(k);

		auto split = [](BigFloat const& x, int32_t k, std::vector<int32_t>& left, std::vector<int32_t>& right) {
			std::vector<int32_t>full(2 * k);
			std::copy(x.m_mantissa.begin(), x.m_mantissa.end(), full.end() - x.m_mantissa.size());
			left = { full.begin(), full.begin() + k };
			right = { full.begin() + k, full.end() };
		};

		split(x, k, vec_a, vec_b);
		split(y, k, vec_c, vec_d);

		BigFloat a(vec_a, k, false);
		BigFloat b(vec_b, k, false);
		BigFloat c(vec_c, k, false);
		BigFloat d(vec_d, k, false);

		BigFloat ac = a * c;
		BigFloat bd = b * d;
		BigFloat mid = (c + d) * (a + b) - (ac + bd);

		return ac.add_exp(2*k) + mid.add_exp(k) + bd;
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Operator that multiplies BigFloat by BigFloat. Implements Caratsuba's algorithm.</para>
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

		auto get_sign = [](BigFloat const& x, BigFloat const& y) {
			return x.m_is_negative ^ y.m_is_negative;
		};

		return mul_fast_0exp(x, y).add_exp( -x_exp_offset - y_exp_offset ).set_negative( get_sign(x, y) ).normalize();
	}

	BigFloat inverse() const;

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Divide BigFloat by BigFloat. Effectively multiplies by inverse number</para>
	/// </summary>
	/// <param name="x">BigFloat dividend</param>
	/// <param name="y">BigFloat divisor</param>
	/// <returns>quotient</returns>
	friend BigFloat operator/(const BigFloat x, const BigFloat y)
	{
		return (x * y.inverse()).normalize();
	}
	
	/// <summary>
	/// author: Odaysky Vladimir
	/// checks if number is zero. 
	/// Required since there are many valid null representations
	/// </summary>
	/// <returns>whether number is equal to zero</returns>
	bool zero() const {
		return m_mantissa.empty() || std::all_of( 
			m_mantissa.begin(), m_mantissa.end(), 
			[](int32_t i) { return i == 0; }
		);
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Checks equality of two BigFloat objects</para>
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

		auto maybe_invert = [this](std::strong_ordering c) -> std::strong_ordering {
		if (m_is_negative)
			return 0 <=> c;
		else 
			return c; 
		};

		if (auto c = m_exponent <=> other.m_exponent; c != 0)
		{
			return maybe_invert(c);
		}
		
		return maybe_invert(std::lexicographical_compare_three_way(m_mantissa.begin(), m_mantissa.end(), other.m_mantissa.begin(), other.m_mantissa.end()));
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Outputs Bigfloat into Stream</para>
	/// </summary>
	/// <param name="os">stream instance to output into</param>
	/// <param name="obj">instance to output</param>
	/// <returns>same stream instance</returns>
	friend std::ostream& operator<<(std::ostream& os, BigFloat const& y)
	{
		return os << y.to_string();
	}

	/// <summary>
	/// <para>author: Odaysky Vladimir</para>
	/// <para>Initializes Bigfloat from Stream</para>
	/// </summary>
	/// <param name="is">stream instance to initialize from</param>
	/// <param name="obj">instance to initialize</param>
	/// <returns>same stream instance</returns>
	friend std::istream& operator>>(std::istream& is, BigFloat& y)
	{	
		std::string repr(std::istreambuf_iterator<char>(is), {});
		y = repr.c_str();
		return is;
	}

	BigFloat& operator += (BigFloat const& b) {
		return *this = *this + b;
	}

	BigFloat& operator -= (BigFloat const& b) {
		return *this = *this - b;
	}

	BigFloat& operator *= (BigFloat const& b) {
		return *this = *this * b;
	}

	BigFloat& operator /= (BigFloat const& b) {
		return *this = *this / b;
	}

	friend BigFloat operator%(BigFloat const&, BigFloat const&);

	BigFloat& operator %= (BigFloat const& b) {
		return *this = *this % b;
	}
};

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// <para>Gets remainder of division of this instance by obj</para>
/// </summary>
/// <param name="obj">BigFloat to divide by this instance</param>
/// <returns>new instance with value of remainder</returns>
BigFloat operator%(BigFloat const& x, BigFloat const& y) { return x - (x / y).floor() * y; }


/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// <para>User-defined literal. effectively calls constructor from string</para>
/// </summary>
/// <param name="x">c-string, number represencation</param>
/// <returns>new instance of bigfloat</returns>
BigFloat operator "" _bf(const char* x) {
	return { x };
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// <para>this function finds where function is zero using half division method</para>
/// </summary>
/// <param name="f">test function</param>
/// <param name="_low">lower border</param>
/// <param name="_high">higher border</param>
/// <param name="_tolerance">result precision</param>
/// <returns>x, where f(x) = 0</returns>
BigFloat find_zero(auto f, BigFloat const& _low, BigFloat const& _high, BigFloat const& _tolerance) {

	BigFloat mid = (_low + _high) * 0.5_bf;

	BigFloat diff = f(_low);
	BigFloat mid_diff = f(mid);

	if (abs(mid_diff) < _tolerance) {
		return mid;
	}

	if (diff * mid_diff <= 0) {
		return find_zero(f, _low, mid, _tolerance);
	}
	return find_zero(f, mid, _high, _tolerance);
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// <para>this method finds inverse value of BigFloat using half division method</para>
/// </summary>
/// <returns>inverse value</returns>
BigFloat BigFloat::inverse() const {
	const static auto tolerance = 0.1e-10_bf;
	if (zero()) 
		throw std::string("division by zero");

	BigFloat tmp = *this;
	int32_t exp_dif = m_exponent;
	
	double sd = m_mantissa.at(0) * INVERSE_BASE;

	if (m_mantissa.size() == 1) {
		return BigFloat(1. / sd).add_exp(-exp_dif);
	}

	tmp.m_exponent -= exp_dif;

	BigFloat low = (double) (1 / sd);
	BigFloat high = (double) (1 / (sd + INVERSE_BASE) );

	return find_zero(
		[tmp](BigFloat const& x) -> BigFloat {return x * tmp - 1; },
		low, high, tolerance
	).add_exp(-exp_dif);
}






