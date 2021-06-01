#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "bigfloat.h"


using namespace std;

int main()
{
    auto n1 = 12353634563535_bf;
    auto n2 = 3456673537735_bf;
    std::cout << n1 / n2;
}