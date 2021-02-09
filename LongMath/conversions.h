#pragma once

#include <iostream>

int getDigit(char c)
{
	return c - 48;
}

char* parseString(int val)
{
	int base = 10;

	int n = val;
	int i = 0;
	while (n)
	{
		n /= base;
		i++;
	}

	char num;
	char* out = new char[i + 1];
	out[i] = 0;
	while (val)
	{
		i--;
		num = val % (base);
		out[i] = 48 + num;
		val /= base;
	}
	return out;
}

int32_t parseInt(char* string)
{
	int32_t res = 0;
	int pow = 1;
	int base = 10;

	char* c;
	for (c = string; *c++;);
	c--; // null does not count

	while (c > string)
	{
		c--;
		res += getDigit(*c) * pow;
		pow *= base;
	}
	return res;
}

