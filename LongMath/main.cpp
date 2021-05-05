#include <iostream>
#include "bigfloat.h"

using namespace std;

int main()
{
	BigFloat res = 5000._bf + 1850._bf;
	BigFloat r = 0.00001_bf - 0.0000001_bf;
	BigFloat r2 = 3213123123.231231231231323_bf + 100000000_bf;
	BigFloat rem = 12.34_bf % 0.037_bf;
	BigFloat in = 123_bf .inverse();
}