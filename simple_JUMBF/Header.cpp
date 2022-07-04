#include "Header.h"
#include "Box.h"
#include <fstream>
#include <vector>
using namespace std;



unsigned int sj_get_byte(unsigned char** buf)
/* Read next input byte; we do not support suspension in this module. */
{

	unsigned int val;
	val = *(*buf)++;
	return val;

}

unsigned int sj_get_2byte(unsigned char** buf)
{
	int val1, val2;
	val1 = (sj_get_byte(buf)) << 8;
	val2 = sj_get_byte(buf);
	return val1 + val2;

}

long sj_get_4byte(unsigned char** buf)
{
	long val1, val2;
	val1 = (sj_get_2byte(buf)) << 16;
	val2 = sj_get_2byte(buf);

	return val1 + val2;
}



unsigned int sj_get_2IIbyte(unsigned char** buf)
{
	int val1, val2;
	val1 = (sj_get_byte(buf));
	val2 = sj_get_byte(buf);
	return val1 + (val2 << 8);
}

long sj_get_4IIbyte(unsigned char** buf)
{
	int val1, val2;
	val1 = (sj_get_2IIbyte(buf));
	val2 = sj_get_2IIbyte(buf);
	return val1 + (val2 << 16);
}

long get_8IIbyte(unsigned char** buf)
{
	int val1, val2;
	val1 = (sj_get_4IIbyte(buf));
	val2 = sj_get_4IIbyte(buf);
	return val1 + (val2 << 32);
}

bool isKthBitSet_1(unsigned char n, int k)
{
	if ((n >> (k - 1)) & 1)
		return true;
	else
		return false;
}
