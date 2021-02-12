#include <iostream>
#include "bigfloat.h"

using namespace std;


int main()
{
	// write tests here
	BigFloat num2 = BigNum(9_91.11_11_1);
	BigFloat num = BigNum   (12.00_23);

	//cout <<"double: " << num.toDouble() << endl;
	cout <<"my repr: " << num2 + num << endl;

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