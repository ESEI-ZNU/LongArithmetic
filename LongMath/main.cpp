#include <iostream>
#include "bigfloat.h"

using namespace std;

int main()
{
	auto a = 123123123152.242_bf;
	auto b = 12312525652.242_bf;
	std::cout << a / b;
}