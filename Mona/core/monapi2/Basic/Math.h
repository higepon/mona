/**
	@file	Math.h
	@brief	数学関連

	数学関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_MATH_H
#define _MONAPI2_BASIC_MATH_H

#include "switch.h"
#include "Type.h"
#include "Limit.h"

namespace monapi2	{

/**
	@brief	数学分野。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/

const double PI		=	3.14159265358979323846;		///<円周率
#ifndef NAPIER
const double NAPIER	=	2.71828182845904523536;		///<自然対数の底
#endif
#ifndef HUGE_VAL
const double HUGE_VAL	=	_DOUBLE_MAX;				///<expなどで計算できる最大値を超えたときに出てくる。
#endif

//四則演算。
void divide(int iNumerator,int iDenominator,int* piQuotient,int* piRemainder);
void divide(double dNumerator,double dDenominator,int* piQuotient,double* pdRemainder);

//指数
double power(double x, double y);
uint power(uint x, uint y);
double power2(double x);
double squareroot(double num);
double log(double x);

#undef log2		  	 	 	//一部の環境ではlog2のマクロが定義されていて衝突するので。 
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

//その他
float convertLinearUnit(float fTarget,float fValue1From,float fValue1To,float fValue2From,float fValue2To);
inline void swap(int* pi1,int* pi2)	{int iTemp=*pi1;*pi1=*pi2;*pi2=iTemp;}

inline int getMin(int i1,int i2)					{return (i1<i2)?i1:i2;}
inline int getMin(uint i1,uint i2)					{return (i1<i2)?i1:i2;}
inline cpchar1 getMin(cpchar1 p1,cpchar1 p2)		{return (p1<p2)?p1:p2;}

inline int getMax(int i1,int i2)					{return (i1>i2)?i1:i2;}
inline int getMax(uint i1,uint i2)					{return (i1>i2)?i1:i2;}
inline cpchar1 getMax(cpchar1 p1,cpchar1 p2)		{return (p1>p2)?p1:p2;}

void putBetween(int* piTarget,int iMin,int iMax);

//dTarget = dModule * q + r;の形にする。
//rはdModule/2より小さくなるように（可能な限り余りの絶対値を少なく）qを1増やすか減らして調整する。
void getModule_ReduceRemainder(double dTarget,double udModule,int* piQuotient,double* piRemainder);


/**
	@brief	2^(実数)を計算するためのサブルーチン
	@date	2005/08/20	junjunn 作成
*/
class SCPowerOf2
{
public:
//2^xを計算。全てのx。
	static double get(double x);

protected:
public:
//2^xを計算。-1<x<1限定
	static double getFloat(double x);
//2^xを計算。xは整数に切り捨てられる。
	static double getInt(double x);
};


/**
	@brief	ランダム生成。
	@date	2005/08/20	junjunn 作成
*/
//多分改良の余地がありそう・・・
class KnuthRandom
{
public:
	KnuthRandom();

	void init(long lSeed);	///<初期化
	void randomize();		///<ランダム生成

	long getInt();			///< 0 <= x < iRANDOM_RANGE の間のlongを返す
	double getFloat();		///< 0 <= x < 1 の間のdoubleを返す

	float getRange()	{return m_fRange;}


protected:
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
