/**
	@file	Math.h
	@brief	数学関連

	数学関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
#ifndef _MONAPI2_BASIC_MATH_H
#define _MONAPI2_BASIC_MATH_H

#include "Type.h"
#include "Limit.h"

namespace monapi2	{

const double PI		=	3.14159265358979323846;		///<円周率
const double NAPIER	=	2.71828182845904523536;		///<自然対数の底
const double HUGE_VAL =	_DOUBLE_MAX;				///<expなどで計算できる最大値を超えたときに出てくる。


//四則演算。
void divide(int iNumerator,int iDenominator,int* piQuotient,int* piRemainder);
void divide(double dNumerator,double dDenominator,int* piQuotient,double* pdRemainder);

//指数
double power(double x, double y);
uint power(uint x, uint y);
double power2(double x);
double squareroot(double num);
double log(double x);
double log2(double x);
double log10(double x);
double exponential(double x);

//三角関数
double sin(double angle);
double cos(double angle);
double tan(double angle);
double atan(double x,double y);
double atan(double x);
double acos(double x);
double asin(double x);

//切り上げ・切り捨て・四捨五入・絶対値など変形や整形
int		mod(int iTarget,int iModule);
double	mod(double iTarget,double dModule);
double	ceil(double x);
double	floor(double x);
double	round(double x);
inline int		absolute(int i)		{return (i>0)?i:-i;}
inline double	absolute(double d)	{return (d>0)?d:-d;}

//乱数
void initRandom(int iSeed);
int getRandom(uint nRange);

float convertLinearUnit(float fTarget,float fValue1From,float fValue1To,float fValue2From,float fValue2To);

//dTarget = dModule * q + r;の形にする。
//rはdModule/2より小さくなるようにqを1増やすか減らして調整する。。
void getModule_ReduceRemainder(double dTarget,double udModule,int* piQuotient,double* piRemainder);

///2^(実数)を計算するためのサブルーチン
class SCPowerOf2
{
public:
//2^xを計算。
	static double get(double x);

protected:
//2^xを計算。-1<x<1限定
	static double getFloat(double x);
//2^xを計算。xは整数に切り捨てられる。
	static double getInt(double x);
};


///ランダムを生成。
///多分改良の余地がありそう・・・
class KnuthRandom
{
public:
	KnuthRandom();

	void init(long lSeed);	///<初期化
	void randomize();		///<ランダム生成

	long getInt();			///< 0 <= x < iRANDOM_RANGE の間のlongを返す
	double getFloat();		///< 0 <= x < 1 の間のdoubleを返す

	float getRange()	{return m_fRange;}


private:
	enum {
		iTABLE_SIZE =55,
		iTABLE_MID_SIZE =24,
		iTABLE_SEED	= 21,
		iRANDOM_RANGE = _INT_MAX,	//0～この範囲で数値が生成される。
	};

	int m_iCurPos;
	float m_fRange;
	long m_aiBuffer[iTABLE_SIZE+1];
};

}	//namespace monapi2

#endif
