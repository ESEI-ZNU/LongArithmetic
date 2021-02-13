#include <iostream>
#include "bigfloat.h"

using namespace std;


int main()
{
	// write tests here
	BigFloat num = BigNum      (12.00_31_23);
	BigFloat num2 = BigNum(9_91_11.11_11_11_1);

	//cout <<"double: " << num.toDouble() << endl;
	BigFloat n3;
	cin >> n3;

	cout <<"my repr: " << n3 << endl;

	const char tests[][10] = { ".123", "312", "12.4312", "312.313", "234.13", "4.134" };

	for (const char* string : tests) 
	{
		cout << string << "\t | " << 
			BigFloat(string).toDouble() << 
			"\t = "  << 
			std::stod(string) << "\t " << 
			(BigFloat(string).toDouble() == std::stod(string)) << endl;
	}
}