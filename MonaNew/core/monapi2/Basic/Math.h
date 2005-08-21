/**
	@file	Math.h
	@brief	数学関連

	数学関連

	License=Mona License
    \version $Revision$
    \date	$Date$
*/
#ifndef _MONAPI2_BASIC_MATH_H
#define _MONAPI2_BASIC_MATH_H

#include "Type.h"
#include "Limit.h"

namespace monapi2	{


const float PI		=	3.14159265358979323846f;		///<円周率
const float NAPIER	=	2.71828182845904523536f;		///<自然対数の底
const double HUGE_VAL =	_DOUBLE_MAX;					///<expなどで計算できる最大値を超えたときに出てくる。


class MathFn
{
public:
//四則演算。
	static void divide(int iNumerator,int iDenominator,int* plQuotient,int* plRemainder);

//指数
	double power(double x, double y);
	uint power(uint x, uint y);
	double squareroot(double num);
	double log(double x);
	double log10(double x);
	double exp(double x);

//三角関数
	double sin(double angle);
	double cos(double angle);
	double tan(double angle);
	double atan(double x, double y);
	double atan(double x);
	double acos(double x);
	double asin(double x);

//切り上げ・切り捨て・四捨五入・絶対値など変形や整形
	double mod(double x, double *buf);
	double ceil(double x);
	double floor(double x);
	double round(double x);
	int absolute(int i);

//乱数
	void initRandom(int seed);
	int getRandom(int Size);
};

class Random
{
public:
	Random();
	Random(unsigned long seed);
	~Random();

public:
	int nextInt();
	double nextDouble();
	void setSeed(unsigned long seed);

//	void init_genrand(unsigned long s);
//	void init_by_array(unsigned long init_key[],int key_length);
//	double genrand_real1(void);
//	unsigned long genrand_int31(void);
};



}	//namespace monapi2

#endif
