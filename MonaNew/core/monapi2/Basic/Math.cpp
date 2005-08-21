/**
	@file	Math.cpp
	@brief	数学関連

	数学関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
#include "switch.h"
#include "Math.h"

namespace monapi2
{

//分子を分母で割って商と剰余を求める。
///@author junjunn @date update:2005/08/20
void divide(int iNumerator,int iDenominator,int* piQuotient,int* piRemainder)
{
	*piQuotient = iNumerator / iDenominator;
	*piRemainder = iNumerator - iDenominator * *piQuotient;
}

//分子を分母で割って商と剰余を求める。
///@author junjunn @date update:2005/08/20
void divide(double dNumerator,double dDenominator,int* piQuotient,double* pdRemainder)
{
	*piQuotient = (int)(dNumerator / dDenominator);
	*pdRemainder = dNumerator - dDenominator * *piQuotient;
}

//e^xを計算。
///@author junjunn @date update:2005/08/20
double exponential(double x)
{
//アセンブラ版
	if (1)
	{
/*
		y=e^x;			求める式。
		e=2^z;			ここでzを導入してこう置くと
	⇒	y=(2^z)^x;
	⇒	y=2^(z*x);

	よってここからyが求められる。
*/
		double f1OverLn2=1.442695041;	//上記のz。1/ln2

		return getPowerOf2(x * f1OverLn2);	//括弧の中が上記のz*x
	}
//小数展開版
	else
	{
/*
e^xのテイラー展開はe^x=1+x+x^2/2! + x^3/3!　・・・

どんなxでも収束するがxが大きいときには時間がかかるので
x = i(整数) + d(小数部分)の形に分解して

  e^x = e^(i + d) = e^i + e^d;

にする。
e^iは整数回数のべき乗なので普通に計算できる。
e^dはテイラー展開で計算する。

これが一般と思われるがそれじゃつまらないのでMonaオリジナルに
x = i(整数) + d1(小数1桁部分) + d(小数2桁部分)まで細かく分解して
速度と精度を稼ぐことにする・・・
*/
		int iXInteger;
		double dXFloat;
		getModule_ReduceRemainder(x,1,&iXInteger,&dXFloat);

//e^iの部分。

//getPowerOf2Intを使えるがまあどうせ非アセンブラ版なので手動でやっちゃえ。
		double dExponentialIntegerResult;
		{
			if (iXInteger>0)
			{
				dExponentialIntegerResult=NAPIER;
				for (int i=0;i<iXInteger-1;i++)
				{
					dExponentialIntegerResult*=NAPIER;
				}
			}
			else
			{
				dExponentialIntegerResult=1;
				for (int i=0;i<-iXInteger;i++)
				{
					dExponentialIntegerResult/=NAPIER;
				}
			}
		}

//小数点1桁部分とそれ以下に分離。
		int iDecimal1;
		double dXFloat2=dXFloat;
		getModule_ReduceRemainder(dXFloat,0.1,&iDecimal1,&dXFloat2);

		double adExponentialDecimal1[]=
		{
			0.40656965974060,	//	e^-0.9
			0.44932896411722,	//	e^-0.8
			0.49658530379141,	//	e^-0.7
			0.54881163609403,	//	e^-0.6
			0.60653065971263,	//	e^-0.5
			0.67032004603564,	//	e^-0.4
			0.74081822068172,	//	e^-0.3
			0.81873075307798,	//	e^-0.2
			0.90483741803596,	//	e^-0.1
			1.0000000000000,	//	e^-0.0
			1.1051709180756,	//	e^ 0.1
			1.2214027581602,	//	e^ 0.2
			1.3498588075760,	//	e^ 0.3
			1.4918246976413,	//	e^ 0.4
			1.6487212707001,	//	e^ 0.5
			1.8221188003905,	//	e^ 0.6
			2.0137527074705,	//	e^ 0.7
			2.2255409284925,	//	e^ 0.8
			2.4596031111569,	//	e^ 0.9
		};

		double dExponentialFloat2Result=0;
		{
			double dProduct = 1;
			double iFactorial=1;
			for (int i=1;i<10;i++)	//10回ぐらいでdoubleの精度が出るみたい。これ以上は無駄。
			{
				dExponentialFloat2Result += dProduct/iFactorial;
				iFactorial*=i;
				dProduct *= (dXFloat2);
			}
		}

		return dExponentialIntegerResult * adExponentialDecimal1[iDecimal1+9] * dExponentialFloat2Result;
	}
}

//dTarget = dModule * q + r;の形にする。
//rはdModule/2より小さくなるようにqを1増やすか減らして調整する。
///@author junjunn @date update:2005/08/20
void getModule_ReduceRemainder(double dTarget,double udModule,int* piQuotient,double* piRemainder)
{
	*piQuotient = (int) (dTarget/udModule);
	*piRemainder = dTarget - udModule * *piQuotient;

//剰余がdMoludeの半分より大きかった場合商の方を一つ進めれる。
	if (dTarget>0)
	{
		if (*piRemainder * 2 > udModule)
		{
			(*piQuotient)++;
			*piRemainder-=(udModule);
		}
	}
	else
	{
		if (-(*piRemainder) * 2 > udModule)
		{
			(*piQuotient)--;
			*piRemainder+=(udModule);
		}
	}
}


//サインを計算する。
///@author junjunn @date update:2005/08/20
double sin(double x)
{
    double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld	x
		fsin
		fstp	qword ptr dResult
	}

	return dResult;
#else
//@author HigePon
    asm volatile ("fsin" : "=t"(dResult) : "0"(x));
    return dResult;
#endif
}

//2^xを計算。
///@author junjunn @date update:2005/08/20
double getPowerOf2(double x)
{
//CPUの命令上小数と整数を一端わけて別々に計算する。
	int iIntPart;
	double dFloatPart;
	divide(x,1,&iIntPart,&dFloatPart);
	double dResultFloat	= getPowerOf2Float(dFloatPart);
	double dResultInt	= getPowerOf2Int(iIntPart);
	return dResultInt * dResultFloat;
}

//getPowerOf2のサブルーチン。2^fを計算。-1<f<1限定
///@author junjunn @date update:2005/08/20
double getPowerOf2Float(double x)
{
	double dResult;

//2^f-1
	__asm
	{
		fld x
		f2xm1
		fstp dResult
	}

	return dResult+1;
}

//getPowerOf2のサブルーチン。2^fを計算。fは整数に切り捨てられる。
///@author junjunn @date update:2005/08/20
double getPowerOf2Int(double x)
{
	double dResult;
	__asm
	{
		fld x
		fld1
		fscale
		fstp dResult
	}

	return dResult;
}

}		//namespace monapi2
