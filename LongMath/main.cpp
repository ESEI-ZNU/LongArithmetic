#include <iostream>
#include "bigfloat.h"

using namespace std;

void runTest(string testName, string operation, bool (*check)(BigFloat, double, BigFloat, double))
{
	int i = 0;

	const char set1[][10] = {
		".123",
		//"312", 
		//"12.4312", 
		//"312.313", 
		//"234.13", 
		"-4.134",
		"1.123"
	};

	const char set2[][10] = {
		"123.31",
		"-123.321",
		"534566.24",
		//".123",
		"-42.234",
		".123",
	};

	for (const char* string : set1)
	{
		BigFloat bf1 = BigFloat(string);
		double dbl1 = std::stod(string);

		for (const char* strng1 : set2)
		{
			double dbl2 = stod(strng1);
			BigFloat bf2 = BigFloat(strng1);

			cout << testName <<" " << i << ": \t"
				<< bf1 << "\t" << operation 
				<< "\t"
				<< bf2 << "\t" <<
				(((*check)(bf1, dbl1, bf2, dbl2)) ? "PASSED" : "\tFAILED") << endl;

			i++;
		}
	}
}

bool checkCmpEq(BigFloat bf1, double db1, BigFloat bf2, double db2) 
{
	return ((bf1 == bf2) == (db1 == db2));
}

bool checkCmpLess(BigFloat bf1, double db1, BigFloat bf2, double db2)
{
	return ((bf1 < bf2) == (db1 < db2));
}

bool checkCmpGrt(BigFloat bf1, double db1, BigFloat bf2, double db2)
{
	return ((bf1 > bf2) == (db1 > db2));
}


int main()
{
	int n = 1;
	BigFloat n1 = "1232.312";
	BigFloat n2 = "312.123";
	cout << n1.mul(n2);


	//runTest("Equality test", "==", checkCmpEq);
	//runTest("Less test", "<", checkCmpLess);
	//runTest("Greater test", ">", checkCmpGrt);

	const char set1[][10] = { 
		".123", 
		//"312", 
		//"12.4312", 
		//"312.313", 
		//"234.13", 
		//"4.134",
		"1.123"
	};

	const char set2[][10] = {
		"123.31",
		//"123.321",
		//"534566.24",
		//".123",
		"42.234"
	};

	cout <<
		endl <<
		string(20, '-') <<
		" Representation and convertions "
		<< string(20, '-')
		<< endl << endl;

	int i = 0;
	for (const char* string : set1)
	{
		BigFloat bf(string);
		double dbl = std::stod(string);

		cout << "[ToDouble Representation test] " << i << ": \t"
			<< bf << "\t== " << dbl << ", \t" << 
			( (dbl == bf.toDouble())?"PASSED" : "FAILED" )<< endl;

		i++;
	}
}