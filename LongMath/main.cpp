#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "bigfloat.h"


using namespace std;
void test_1() {
	auto a = 12345678.912355677_bf;
	auto b = 5467300849.55555633_bf;
	cout << a << " + " << b << " = " << a + b << endl;
}
void test_2() {
	auto a = 12345678.912355677_bf;
	auto b = 5467300849.55555633_bf;
	cout << a << " * " << b << " = " << a * b << endl;
}
void test_3() {
	auto a = 12345678.912355677_bf;

	cout << a << " inverse -> " <<a.inverse() << endl;
}
void test_4() {
	auto a = 12345678.912355677_bf;

	cout  << " number * inverse -> " << a.inverse()* a << endl;
}
void test_5() {
	auto a = 12345678.912355677_bf;
	auto b = 5467300849.55555633_bf;
	cout << a << " / " << b << " = " << a / b << endl;
}
void test_6() {
	BigFloat rez = 0;
	BigFloat x = 1.57079632679_bf;
	BigFloat fact = 1;
	BigFloat exp = x;
	BigFloat prescision = 0.0001_bf;
	BigFloat a = 100;
	int sign = 1;
	for (BigFloat n = 0_bf; a > prescision; n++) {
		a = sign * exp / fact;
		sign = -sign;
		rez += a;
		BigFloat temp = (2 * (n + 1) + 1);
		fact = fact * (temp - 1) * temp;
		exp *= (x * x);
	}
	cout << rez;

}
int main()
{
	std::cout << "Hello, world!";   
	BigFloat a = 4677837363_bf;
	BigFloat b = 6783837564_bf;
	cout << a / b;
	test_6();
}