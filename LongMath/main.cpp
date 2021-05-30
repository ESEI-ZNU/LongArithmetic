#include <iostream>
#include "bigfloat.h"

using namespace std;

int main()
{
	auto a = 1436873960.80532050977130886_bf;
	auto b = 49758.94930977615307528598175_bf;
	std::cout << '\n' << a << " + " << b << " =\n= " << a + b << '\n';
}