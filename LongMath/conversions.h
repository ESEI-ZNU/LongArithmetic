#pragma once
#include <iostream>
#include <vector>
#include <regex>

#define BASE_POW 9
#define BIG_FLOAT_PARSE_REGEX "([+-])?(((\\d+)(\\.(\\d*))?)|(\\.(\\d+)))([eE]([+-])?(\\d+))?"

const int32_t BASE = pow(10, BASE_POW);
const double INVERSE_BASE = 1 / (double)BASE;

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// count digits of given object 
/// that supports division operation
/// </summary>
/// <typeparam name="T">division-supporting object type</typeparam>
/// <param name="l">obejct itself</param>
/// <param name="base">number base</param>
/// <returns>number of digits in given object</returns>
template <typename T>
int count_digits(T l, int base = 10)
{
	int i = 1;
	for (; l /= base; i++);
	return i;
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// parses bigfloat components into double type
/// </summary>
/// <param name="m_mantissa">bigfloat's mantissa</param>
/// <param name="m_exponent">bigfloat's exponent</param>
/// <param name="base">number base</param>
/// <returns>double representation of bigfloat</returns>
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

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns>maximum of two absolute values</returns>
template<typename T>
T max_abs(T x, T y) {
	return (std::max(abs(x), abs(y)) == abs(x)) ? x : y;
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// Validate and normalize BigFloat representation
/// </summary>
/// <param name="s">input string</param>
/// <returns>output string</returns>
std::string normalize_repr(char const* s)
{
	std::string str = s;
	while (!str.empty() && str.front() == '0')str.erase(0, 1);
	remove_if(str.begin(), str.end(), [](char x) {return x == '_' || x == '\''; });
	return str;
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// parse normalized string into array of digits
/// </summary>
/// <param name="string">number representation</param>
/// <returns>array of digits in given base</returns>
std::vector<int32_t>parse_digits(std::string string) {
	std::vector<int32_t> digits;

	if (string.empty())
		return digits;

	int digit_index = 0;
	std::string digit(BASE_POW, '0');

	for (char c : string) {
		digit[digit_index % BASE_POW] = c;

		if (digit_index != 0 && (digit_index + 1) % BASE_POW == 0) {
			digits.push_back(std::stoi(digit));
			digit = std::string(BASE_POW, '0');
		}
		digit_index++;
	}

	return digits;
}

/// <summary>
/// <para>author: Odaysky Vladimir</para>
/// parse string representation of bigfloat that matches regex into separate components
/// </summary>
/// <param name="str">string representation</param>
/// <returns>tuple of is_negative, mantissa and exponent</returns>
std::tuple<bool, std::vector<int32_t>, int32_t> parse_bigfloat(std::string const& str) {
	std::regex re(BIG_FLOAT_PARSE_REGEX);
	std::smatch match;

	if (!str.length()) {
		return { false, {}, 0 };
	}

	if (!std::regex_search(str, match, re)) {
		throw std::exception("Invalid Format");
	}

	std::string sign = match.str(1);
	std::string whole_part = match.str(4);
	std::string frac_part = match.str(6).size() == 0 ? match.str(8) : match.str(6);
	std::string exp_sign = match.str(10);
	std::string exp = match.str(11);

	int whole_diff = BASE_POW - whole_part.size() % BASE_POW;
	whole_part = std::string(whole_diff, '0') + whole_part;
	int frac_diff = BASE_POW - frac_part.size() % BASE_POW;
	frac_part += std::string(frac_diff, '0');
	std::vector<int32_t> mantissa = parse_digits(whole_part);
	int32_t exponent = mantissa.size();
	std::vector<int32_t> vfrac_part = parse_digits(frac_part);
	mantissa.insert(mantissa.end(), vfrac_part.begin(), vfrac_part.end());
	int32_t exp_literal = exp.size() ? (stoi(exp) * (((exp_sign != "-") * 2) - 1)) : 0;
	return { sign == "-", mantissa, exp_literal + exponent };
}