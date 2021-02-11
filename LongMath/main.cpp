#include <iostream>
#include "bigfloat.h"

using namespace std;

int main()
{
	// write tests here
	// 1  23 12 34 23 42  4.4 23 42 43
	// 11 11 11 11 11 1.1 11  11 11 00 00
	BigFloat num = BigNum (99_23_12_34_23_42_4.4_23_42_43);
	BigFloat num2 = BigNum(99_11_11_11_11_11_1.1_11_11_11);

	cout <<"n: " << num + num2;
	double d = 123'456.789'012;
}