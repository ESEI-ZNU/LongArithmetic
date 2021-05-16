#ifndef BIGFLOAT_H
#define BIGFLOAT_H
#include <cmath>
#include <iterator>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include "conversions.h"

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
	int divDigits;
	bool isNeg = false;
	std::vector<std::int32_t> mantissa;
	static const int len_f_naive = 3;
	int32_t exponent = 0;
	int count_;

public:

	// default constructor
	BigFloat() { divDigits = 10; count_ = 0; };

	// string constructor
	BigFloat(const char* repr)
	{
		count_ = 0;
		*this = repr;
	}

	// numeric constructor
	BigFloat(double value)
	{
		divDigits = 10;
		count_ = 0;
		*this = std::to_string(value).c_str();
	}
	BigFloat(std::vector<std::int32_t> mantis) :
		mantissa(mantis),
		exponent(0)
	{
		count_ = 0;
	}
	// copy constructor
	BigFloat(const BigFloat& obj)
	{
		this->exponent = obj.exponent;
		this->mantissa = obj.mantissa;
		this->isNeg = obj.isNeg;
		divDigits = obj.divDigits;
		count_ = 0;
	}

	BigFloat operator = (const char* repr)
	{
		count_ = 0;
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
		divDigits = 10;
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

	void abs()
	{
		this->isNeg = false;
	}

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
	//Diana
	void setExp(int exp)
	{
		exponent = exp;
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

	//This is my job(Diana)
	BigFloat caratsuba(BigFloat& x, const BigFloat& y)
	{

		int exp = x.exponent + y.exponent;

		auto len = x.mantissa.size();
		vector<int> res(2 * len);

		if (len <= len_f_naive) {
			return x * y;
		}

		auto k = len / 2;

		vector<int32_t> Xr{ x.mantissa.begin(), x.mantissa.begin() + k };
		vector<int32_t> Xl{ x.mantissa.begin() + k, x.mantissa.end() };
		vector<int32_t> Yr{ y.mantissa.begin(), y.mantissa.begin() + k };
		vector<int32_t> Yl{ y.mantissa.begin() + k, y.mantissa.end() };
		BigFloat Xln(Xl);
		BigFloat Xrn(Xr);
		BigFloat Yln(Yl);
		BigFloat Yrn(Yr);
		count_++;
		BigFloat P1 = caratsuba(Xln, Yln);
		count_--;
		count_++;
		BigFloat  P2 = caratsuba(Xrn, Yrn);

		count_--;
		vector<int32_t> Xlr(k);
		vector<int32_t> Ylr(k);

		for (int i = 0; i < k; ++i) {
			Xlr[i] = Xln.mantissa[i] + Xrn.mantissa[i];
			Ylr[i] = Yln.mantissa[i] + Yrn.mantissa[i];
		}
		BigFloat Xlrn(Xlr); BigFloat Ylrn(Ylr);
		count_++;
		BigFloat P3 = caratsuba(Xlrn, Ylrn);
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
			resn.setExp(exp);
		return resn;

	}
	BigFloat& operator*(const BigFloat obj)
	{
		// code that will multiply this by obj
		size_t len = mantissa.size() + obj.mantissa.size(); // максимальная длина нового числа не больше суммы длин перемножаемых чисел

		BigFloat res; // создадим объект для результата

		if ((obj.isNeg && !isNeg) || (!obj.isNeg && isNeg))
			res.isNeg = true;
		// res.sign = sign * x.sign; // перемножаем знаки
		res.mantissa = std::vector<int>(len, 0); // создаём вектор из нулей
		res.exponent = exponent + obj.exponent; // складываем экспоненты

		// умножаем числа в столбик
		for (size_t i = 0; i < mantissa.size(); i++)
			for (size_t j = 0; j < obj.mantissa.size(); j++)
				res.mantissa[i + j + 1] += mantissa[i] * obj.mantissa[j]; // прибавляем результат произведения цифр из i-го и j-го разрядов

		// в результате такого перемножения в ячейках могли получиться двузначные числа, поэтому нужно выполнить переносы
		for (size_t i = len - 1; i > 0; i--) {
			res.mantissa[i - 1] += res.mantissa[i] / 10; // добавляем к более старшему разряду десятки текущего разряда
			res.mantissa[i] %= 10; // оставляем только единицы у текущего разряда
		}

		//res.removeZeroes(); // удаляем нули, как в конструкторе из строки, если таковые имеются

	   // return res;
		*this = res;
		return this->normalize();
	}
	BigFloat inverse() const {
		if (mantissa.size() == 1 && mantissa[0] == 0)
			throw std::string("LongDouble LongDouble::inverse() - division by zero!"); // делить на ноль нельзя, поэтому бросим исключение

		BigFloat x(*this); // скопируем число,
		x.isNeg = false; // сделав его положительным

		BigFloat d(1.0); // создадим то, что будем делить на x

		BigFloat res; // создадит объект для результата
		res.isNeg = isNeg; // знак результата совпадёт со знаком числа
		res.exponent = 1; // начнём с единичной экспоненты
		res.mantissa = std::vector<int>(); // создадим пустой вектор для цифр обратного элемента

		// пока число меньше 1
		BigFloat b(1.0);
		while (x < b) {
			x.exponent++; // будем увеличивать его экспоненту (умножать на 10 фактически)
			res.exponent++; // и заодно экспоненту результата

		}

		// дальше сдлеаем число d большим x, также умножая его на 10, чтобы получить число 100...0
		while (d < x)
			d.exponent++;

		res.exponent -= d.exponent - 1; // подсчитаем реальное количество цифр в целой части

		size_t numbers = 0; // количество уже вычисленных цифр дробной части
		size_t totalNumbers = divDigits + std::max((int)0, res.exponent); // количество цифр с учётом целой части
		BigFloat z(0.0);
		do {
			int div = 0; // будущая цифра

			// считаем, сколько раз нужно вычесть x из d
			while (d >= x) {
				div++;
				d = d - x;
				//cout
				//cout
				//std::cout<<"d="<<d<<std::endl;
				//std::cout<<"x="<<x<<std::endl;
			}
			//std::cout<<"ok\n";
						// увеличиваем остаток в 10 раз
			d.exponent++;
			// d.removeZeroes();
			d.normalize();
			res.mantissa.push_back(div); // записываем сформированную цифру
			numbers++; // увеличиваем число вычисленных цифр

		} while (d != z && numbers < totalNumbers); // считаем до тех пор, пока не дойдём до нулевого остатка или пока не превысим точность

		return res; // возвращаем результат
	}

	//This is my job(Diana)
	BigFloat& operator/(const BigFloat obj)
	{
		BigFloat res = *this * obj.inverse(); // выполняем умножение на обратный элемент

		   // избавляемся от записи вида d...dd(9), которая эквивалентна d...dd+1
		size_t i = res.mantissa.size() - 1 - std::max(0, exponent);
		size_t n = std::max(0, res.exponent);

		// если в указанном месте девятка, то ищем место, в котором девятки закончатся
		if (i > n && res.mantissa[i] == 9) {
			while (i > n && res.mantissa[i] == 9)
				i--;

			// если дошли до целой части
			if (res.mantissa[i] == 9) {
				res.mantissa.erase(res.mantissa.begin() + n, res.mantissa.end());
				// то удаляем всю вещественную часть
				BigFloat u(1.0);
				if (res.isNeg)
					res = res - u; // и прибавляем 1 (или -1 к отрицательному)
				else
					res = res + u;
			}
			else {
				res.mantissa.erase(res.mantissa.begin() + i + 1, res.mantissa.end()); // иначе обрезаем с найденного места
				res.mantissa[i]++; // и увеличиваем на 1 текущий разряд
			}
		}

		//return res;
	// code that will divide this by obj
		*this = res;
		// code that will divide this by obj
		return this->normalize();
	}

	BigFloat& operator%(const BigFloat obj)
	{
		// code that will take remainder from dividing this by obj
		return this->normalize();
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
			diff.abs();
			BigFloat maxDiff = BigNum(0.000001);
			return diff.normalize() < maxDiff.normalize();
		}

		return true;
	}

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
#endif // BIGFLOAT_H
