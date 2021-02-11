#include <iostream>
#include "bigfloat.h"

using namespace std;


int main()
{
	// write tests here
	// 1  23 12 34 23 42  4.4 23 42 43
	// 11 11 11 11 11 1.1 11  11 11 00 00
	// 0.01 23 01 23 x100e2
	BigFloat num = BigNum (123.123);
	BigFloat num2 = BigNum(99_11_11_11_11_11_1.1_11_11_11);

	cout <<"double: " << num .toDouble() << endl;
	cout <<"my repr: " << num;
}