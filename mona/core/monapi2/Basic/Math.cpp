/**
	@file	Math.cpp
	@brief	数学関連

	数学関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Math.h"
#include "Debug.h"

namespace monapi2
{
//乱数に使うオブジェクト
KnuthRandom g_KnuthRandom;

const double g_cd1overLn2	= 1.4426950408890;	// 1/ln2
const double g_cdLn2		= 0.69314718055995;	// ln2



/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void divide(int iNumerator,int iDenominator,int* piQuotient,int* piRemainder)
{
	*piQuotient = iNumerator / iDenominator;
	*piRemainder = iNumerator - iDenominator * *piQuotient;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void divide(double dNumerator,double dDenominator,int* piQuotient,double* pdRemainder)
{
	*piQuotient = (int)(dNumerator / dDenominator);
	*pdRemainder = dNumerator - dDenominator * *piQuotient;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
float convertLinearUnit(float fTarget,float fValue1From,float fValue1To,float fValue2From,float fValue2To)
{
	return (fValue2To - fValue1To) / (fValue2From - fValue1From) * (fTarget-fValue1From)+ fValue1To;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void putBetween(int* piTarget,int iMin,int iMax)
{
	*piTarget=getMin(*piTarget,iMax);
	*piTarget=getMax(*piTarget,iMin);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
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

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int mod(int iTarget,int iModule)
{
	int i=iTarget % iModule;
	return i;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double mod(double dTarget,double dModule)
{
	int iQuotient;
	double dRemainder;
	divide(dTarget,dModule,&iQuotient,&dRemainder);

	return dRemainder;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double floor(double x)
{
	return (x>=0)?(int)x:(int)x-1;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double ceil(double x)
{
	return (x>=0)?(int)x+1:(int)x;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double round(double x)
{
	return (x>=0)?floor(x+0.5):ceil(x-0.5);	//0.5を足して切り捨ては古典的なトリック。
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double log2(double x)
{
	if (x < 0)	return 0;

	double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld1
		fld		x
		fyl2x
		fstp	dResult
	}
#else
	asm volatile ("fld1");
    asm volatile ("fyl2x" : "=t"(dResult) : "0"(x));
#endif

	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double log10(double x)
{
/*
		10^y=x;			求める式。
		10=2^z;			zを導入してこう置く。
	⇒	(2^z)^y=x;
	⇒	2^(z*y)=x;
	⇒	z*y=log2(x);
	⇒	y=log2(x)/z;
*/

//上記の1/z。zで割るより1/zをかける方がCPU的に速いのでこう置いとく。
	const double cd1overLog2of10 = 0.30102999566398;
	double dResult = log2(x) * cd1overLog2of10;
	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double log(double x)
{
/*
		e^y=x;			求める式。
		e=2^z;			zを導入してこう置く。
	⇒	(2^z)^y=x;
	⇒	2^(z*y)=x;
	⇒	z*y=log2(x);
	⇒	y=log2(x)/z;
*/
	double dResult = log2(x) * g_cdLn2;
	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double exponential(double x)
{
//アセンブラ版
#if 1
	{
/*
		y=e^x;			求める式。
		e=2^z;			zを導入してこう置く。
	⇒	y=(2^z)^x;
	⇒	y=2^(z*x);
*/
		return SCPowerOf2::get(g_cd1overLn2 * x);	//括弧の中が上記のz*x
	}
//小数展開版
#else
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
		double dProduct = 1;
		double iFactorial=1;
		for (int i=1;i<10;i++)	//10回ぐらいでdoubleの精度が出るみたい。これ以上は無駄。
		{
			dExponentialFloat2Result += dProduct/iFactorial;
			iFactorial*=i;
			dProduct *= (dXFloat2);
		}

		return dExponentialIntegerResult * adExponentialDecimal1[iDecimal1+9] * dExponentialFloat2Result;
	}
#endif	//if 1
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint power(uint x, uint y)
{
	uint nResult=1;
	for (uint n=0;n<y;n++)
	{
		nResult*=x;
	}
	return nResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double power2(double x)
{
	return SCPowerOf2::get(x);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double power(double x, double y)
{
/*
		z=x^y;					求める式。
	⇒	log2(z)=y*log2(x);
	⇒	z = 2^(y*log2(x))
*/
	double dResult = power2(y * log2(x));
	return dResult;
}


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double squareroot(double x)
{
	if (x<0)	return 0;

    double dResult;
#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		fsqrt
		fstp	dResult
	}
#else
//@author Higepon
    asm volatile ("fsqrt" : "=t"(dResult) : "0"(x));
#endif
    return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double sin(double x)
{
    double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		fsin
		fstp	dResult
	}
#else
//@author Higepon
    asm volatile ("fsin" : "=t"(dResult) : "0"(x));
#endif

	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double cos(double x)
{
    double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		fcos
		fstp	dResult
	}
#else
//@author Higepon
    asm volatile ("fcos" : "=t"(dResult) : "0"(x));
#endif

	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double tan(double x)
{
    double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		fptan
		fstp	st					//sinとかと違い結果はst(1)に入っているのでstは切り捨て
		fstp	dResult
	}
#else
    asm volatile ("fptan" : "=t"(dResult),"=u"(dResult) : "0"(x));
#endif

	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double atan(double y,double x)
{
    double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		y
		fld		x
		fpatan
		fstp	dResult
	}
#else
//@author Higepon
    asm volatile("fpatan" : "=t" (dResult) : "0" (x), "u" (y) : "st(1)");
#endif

	return dResult;
}

double atan(double x)
{
	return atan(x,1);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double asin(double x)
{
	if (absolute(x) > 1)	return 0;
/*
			1=sin(x)^2+cos(x)^2
	同時に	tan(x)=sin(x)/cos(x)	なので
	⇒		1=(sin(x)^2+(sin(x)/tan(x))^2
	⇒		tan(x) = √(sin(x)^2 / (1-sin(x)^2))

	ここで命題により	sin(y)=x;
	⇒		tan(y) = √(sin(y)^2 / (1-sin(y)^2))
	⇒		tan(y) = √(x^2 / (1-x^2))
	⇒		y = atan(√(x^2 / (1-x^2))
*/

	double d = x*x;
	double dResult = atan(squareroot(d / (1-d)));

	return (x>0)?dResult:-dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double acos(double x)
{
	if (absolute(x) > 1)	return 0;
	if (x==-1)	return PI;
/*
			1=sin(x)^2+cos(x)^2
	同時に	tan(x)=sin(x)/cos(x)	なので
	⇒		1=(tan(x)cos(x))^2+cos(x)^2
	⇒		tan(x) = √((1-cos(x)^2)/cos(x)^2)
	⇒		tan(x) = √(1/cos(x)^2 - 1)

	ここで命題により	cos(y)=x;
	⇒		tan(y) = √(1/cos(y)^2 - 1)
	⇒		tan(y) = √(1/x^2 - 1)
	⇒		y = atan(√(1/x^2 - 1))
*/

	double d = x;
	double dResult = atan(squareroot(1.0/(d*d) -1));

	return (x>0)?dResult:(PI-dResult);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
double SCPowerOf2::get(double x)
{
//CPUの命令上小数と整数をわけて別々に計算する。
	int iIntPart;
	double dFloatPart;
	divide(x,1,&iIntPart,&dFloatPart);
	double dResultFloat	= getFloat(dFloatPart);
	double dResultInt	= getInt(iIntPart);
	double dResult = dResultInt * dResultFloat;
	return dResult;
}

/**
	@brief	2^fを計算。-1<f<1限定。
	@date	2005/08/20	junjunn 作成
*/
double SCPowerOf2::getFloat(double x)
{
	double dResult;

//2^f-1
#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		f2xm1
		fstp	dResult
	}
#else
    asm volatile ("f2xm1" : "=t"(dResult) : "0"(x));
#endif

	return dResult+1;
}

/**
	@brief	2^fを計算。fは整数に切り捨てられる。
	@date	2005/08/20	junjunn 作成
*/
//st(0) * 2^(st(1)を計算する。
double SCPowerOf2::getInt(double x)
{
	double dResult;

#ifdef _COMPILER_VISUALC
	__asm
	{
		fld		x
		fld1
		fscale
		fstp	st(1)	//結果はst(0)にあるのだがどうせst(1)をポップする必要があるのでst(1)に移動。
		fstp	dResult
	}
#else
  	asm volatile ("fld %0"::"f"(x));
  	asm volatile ("fld1");
	asm volatile ("fscale;" : "=t"(dResult));
#endif

	return dResult;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void initRandom(int iSeed)
{
	g_KnuthRandom.init(iSeed);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int getRandom(uint nRange)
{
	int r = g_KnuthRandom.getInt();

	return ((int) (r/g_KnuthRandom.getRange()*nRange));
}

//KnuthRandom////////////////
/**
	@brief	コンストラクタ
	@date	2005/08/20	junjunn 作成
*/
KnuthRandom::KnuthRandom()
{
	m_iCurPos=0;
	m_fRange = (float)iRANDOM_RANGE;
}

/**
	@brief	初期化
	@date	2005/08/20	junjunn 作成
*/
void KnuthRandom::init(long lSeed)
{
	m_aiBuffer[iTABLE_SIZE] = lSeed;
	long r = 1;
	int i;
	for (i=1;i<iTABLE_SIZE;i++) 
	{
		int iPos = (iTABLE_SEED * i) % iTABLE_SIZE;
		m_aiBuffer[iPos] = r;
		r = lSeed - r;
		if (r < 0) r += iRANDOM_RANGE;
		lSeed = m_aiBuffer[iPos];
	}

//最初に転がす。
	for (i=0;i<3;i++)		randomize();
	m_iCurPos = iTABLE_SIZE;
}

/**
	@brief	ランダム発生
	@date	2005/08/20	junjunn 作成
*/
void KnuthRandom::randomize()
{
	int i;
	for (i = 1; i <= iTABLE_MID_SIZE; i++) 
	{
		int r = m_aiBuffer[i] - m_aiBuffer[i + iTABLE_MID_SIZE+7];
		if (r < 0) r += iRANDOM_RANGE;
		m_aiBuffer[i] = r;
	}
	for (i = iTABLE_MID_SIZE+1; i <= iTABLE_SIZE; i++) 
	{
		int r = m_aiBuffer[i] - m_aiBuffer[i - iTABLE_MID_SIZE];
		if (r < 0) r += iRANDOM_RANGE;
		m_aiBuffer[i] = r;
	}
}

/**
	@brief	intを取り出す。
	@date	2005/08/20	junjunn 作成
*/
long KnuthRandom::getInt()
{
	if (++m_iCurPos > iTABLE_SIZE) 
	{
		randomize();
		m_iCurPos = 1;  
	}
	return m_aiBuffer[m_iCurPos];
}

/**
	@brief	floatを取り出す。
	@date	2005/08/20	junjunn 作成
*/
double KnuthRandom::getFloat()
{
	return (1.0 / iRANDOM_RANGE) * getInt();
}

}		//namespace monapi2
