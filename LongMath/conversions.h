#pragma once
#include <iostream>
#include <vector>
#include <regex>

#define BASE_POW 2
#define BIG_FLOAT_PARSE_REGEX "([+-])?(((\\d+)(\\.(\\d*))?)|(\\.(\\d+)))([eE]([+-])?(\\d+))?"

const int32_t BASE = pow(10, BASE_POW);
const double INVERSE_BASE = 1 / (double)BASE;

template <typename T>
int count_digits(T l, int base = 10)
{
	int i = 1;
	for (; l /= base; i++);
	return i;
}


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

template<typename T>
T max_abs(T x, T y) {
	return (std::max(abs(x), abs(y)) == abs(x)) ? x : y;
}

/// <summary>
	/// Validate and normalize BigFloat representation
	/// </summary>
	/// <param name="s">input string</param>
	/// <returns>output string</returns>
std::string normalize_repr(char const* s)
{
	std::string str = s;
	while (str.front() == '0')str.erase(0, 1);
	remove_if(str.begin(), str.end(), [](char x) {return x == '_' || x == '\''; });
	return str;
}

std::vector<int32_t>parse_digits(std::string string) {
	std::vector<int32_t> digits;

	if (string.empty())
		return digits;

	int digit_index = 0;
	std::string digit(BASE_POW, '0');

	for (char c : string) {
		digit[ digit_index % BASE_POW ] = c;

		if (digit_index != 0 && (digit_index+1) % BASE_POW == 0) {
			digits.push_back(std::stoi(digit));
			digit = std::string(BASE_POW, '0');
		}
		digit_index++;
	}

	return digits;
}

std::tuple<bool, std::vector<int32_t>, int32_t> parse_bigfloat(std::string const& str) {
	std::regex re(BIG_FLOAT_PARSE_REGEX);
	std::smatch match;

	if (!std::regex_search(str, match, re)) {
		throw ParseException("Invalid Format");
	}

	std::string sign = match.str(1);
	std::string whole_part = match.str(4);
	std::string frac_part = match.str(6).size() == 0 ? match.str(8) : match.str(6);
	std::string exp_sign = match.str(10);
	std::string exp = match.str(11);
	
	int whole_diff = whole_part.size() % BASE_POW;
	whole_part = std::string(whole_diff, '0') + whole_part;
	int frac_diff = frac_part.size() % BASE_POW;
	frac_part += std::string(frac_diff, '0');
	std::vector<int32_t> mantissa = parse_digits(whole_part);
	int32_t exponent = mantissa.size();
	std::vector<int32_t> vfrac_part = parse_digits(frac_part);
	mantissa.insert(mantissa.end(), vfrac_part.begin(), vfrac_part.end());
	int32_t exp_literal = exp.size() ? (stoi(exp) * (((exp_sign != "-") * 2) - 1) ) : 0;
	return { sign == "-", mantissa, exp_literal + exponent };
}

