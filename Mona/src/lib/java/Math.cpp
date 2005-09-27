// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <java/lang/Math.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <math.h>
#endif

jdouble
java::lang::Math::sqrt (jdouble a)
{
	return ::sqrt(a);
}


jdouble
java::lang::Math::sin (jdouble a)
{
	return ::sin(a);
}


jdouble
java::lang::Math::cos (jdouble a)
{
	return ::cos(a);
}


jdouble
java::lang::Math::tan (jdouble a)
{
	return ::tan(a);
}


jdouble
java::lang::Math::atan (jdouble a)
{
	return ::atan(a);
}


jdouble
java::lang::Math::atan2 (jdouble y, jdouble x)
{
	return ::atan2(y, x);
}


jdouble
java::lang::Math::pow (jdouble a, jdouble b)
{
	return ::pow(a, b);
}


