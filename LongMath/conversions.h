#pragma once

#include <iostream>

int getDigit(char c)
{
	return c - 48;
}

int countDigits(long l) 
{
	int base = 10;
	int i = 1;
	for (; l /= base; i++);
	return i;
}

char* parseString(int val, int i) 
{
	int base = 10;

	char* out = new char[i + 1];
	out[i] = 0;

	while (val)
	{
		i--;
		char num = val % (base);
		out[i] = 48 + num;
		val /= base;
	}

	// fill left digits with 0
	while (i > 0)
	{
		i--;
		out[i] = '0';
	}
	return out;
}

char* parseString(int val)
{
	int base = 10;
	int i = countDigits(val);

	char num;
	char* out = new char[i + 1];
	out[i] = 0;
	out[0] = '0';
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

int32_t parseInt(int start, char* string)
{
	return parseInt(start + string);
}

void concat(char* dest, char* src)
{
	for (; *dest; dest++);
	for (; *src; *dest++ = *src++);
	*dest = 0;
}

int strlen(char* s) 
{
	int len = 0;
	for (; *s++; len++);
	return len;
}

