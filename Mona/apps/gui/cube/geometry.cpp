/*

ƒRƒƒ“ƒg’†‚ÌAm,q,v‚Í
m = Matrix
q = Quaternion
v = Vector
‚ğˆÓ–¡‚·‚éB

*/

/*
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
*/
#include <monapi.h>
using namespace MonAPI;

#include "Geometry.h"


float atan2(float a,float b){
    return 0.0f; // ‚Ä‚«‚Æ[B
}
float acos(float a){
    return 0.0f; // ‚Ä‚«‚Æ[B
}

void memset(void *p,int val,int len){
    int i;
    for(i=0;i<len;i++){
        ((unsigned char *)p)[i] = val;
    }
}

Matrix::Matrix()
{
    memset(m,0,sizeof(float)*16);
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
}

Matrix::~Matrix()
{
}

void
Matrix::put()
{
	int i,t;
/*
	printf("{\n");
	for(t=0;t<4;t++)
	{
		printf("  ");
		for(i=0;i<4;i++)
		{
			printf("%11.6f,",m[t][i]);
		}
		printf("\n");
	}
	printf("}\n");*/
}

void
Matrix::SetXrotate(float r)
{
	memset(m,0,sizeof(float)*16);

	m[1][1] =   m[2][2] = (float)cos(r);
	m[2][1] = -(m[1][2] = (float)sin(r));
	m[0][0] =   m[3][3] = 1.0f;
}
void
Matrix::SetXrotate(float s,float c)
{
	memset(m,0,sizeof(float)*16);

	m[1][1] =   m[2][2] = (float)c;
	m[2][1] = -(m[1][2] = (float)s);
	m[0][0] =   m[3][3] = 1.0f;
}

void
Matrix::SetYrotate(float r)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] =   m[2][2] = (float)cos(r);
	m[0][2] = -(m[2][0] = (float)sin(r));
	m[1][1] =   m[3][3] = 1.0f;
}
void
Matrix::SetYrotate(float s,float c)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] =   m[2][2] = (float)c;
	m[0][2] = -(m[2][0] = (float)s);
	m[1][1] =   m[3][3] = 1.0f;
}

void
Matrix::SetZrotate(float r)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] =   m[1][1] = (float)cos(r);
	m[1][0] = -(m[0][1] = (float)sin(r));
	m[2][2] =   m[3][3] = 1.0f;
}
void
Matrix::SetZrotate(float s,float c)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] =   m[1][1] = (float)c;
	m[1][0] = -(m[0][1] = (float)s);
	m[2][2] =   m[3][3] = 1.0f;
}

void
Matrix::SetXrotateF(float r)
{
	m[1][1] =   m[2][2] = (float)cos(r);
	m[2][1] = -(m[1][2] = (float)sin(r));
}

void
Matrix::SetYrotateF(float r)
{
	m[0][0] =   m[2][2] = (float)cos(r);
	m[0][2] = -(m[2][0] = (float)sin(r));
}

void
Matrix::SetZrotateF(float r)
{
	m[0][0] =   m[1][1] = (float)cos(r);
	m[1][0] = -(m[0][1] = (float)sin(r));
}

void
Matrix::SetMagnification(float src)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] = m[1][1] = m[2][2] = (float)src;
	m[3][3] = 1;
}

void
Matrix::SetMagnification(float x,float y,float z)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] = (float)x;
	m[1][1] = (float)y;
	m[2][2] = (float)z;
	m[3][3] = 1;
}

void
Matrix::SetMagnification(Vector &src)
{
	memset(m,0,sizeof(float)*16);

	m[0][0] = src.x;
	m[1][1] = src.y;
	m[2][2] = src.z;
	m[3][3] = 1;
}

// (0,0,1)‚ªA(ix¤iy¤iz)‚ğã(y‚ª³)‚É‚µ‚ÄA(x¤y¤z)‚ğŒü‚­Matrix‚ğ¶¬
void
Matrix::SetOrientation(float x,float y,float z,float ix,float iy,float iz)
{
// x,y‚Ì‡
	float rx=.0,ry=.0,rz=.0;
	float d,s,c;

// x²
	if(y!=0 || z!=0)
	{
		// x²‚ÌŒX‚«‚ğ“¾‚é
		rx = atan2(y,z);

		// x²‚Å“|‚·
		s = sin(rx);
		c = cos(rx);

		// y‚Í‚±‚ÌŒã•K—v–³‚¢‚Ì‚ÅŒvZ‚µ‚È‚¢
		z = NearZero(c*z + s*y);

		// p¨‰ñ“]
		d = iz;
		iz = NearZero(c*d  + s*iy);
		iy = NearZero(c*iy - s*d);
	}

// y²
	if(x!=0 || z!=0)
	{
		// y²‚ÌŒX‚«‚ğ“¾‚é
		ry = atan2(z,x) - PI2;

		// iz‚Í‚±‚ÌŒã•K—v–³‚¢‚Ì‚ÅŒvZ‚µ‚È‚¢
		ix = NearZero(cos(ry)*ix + sin(ry)*iz);
	}

// z²ip¨‰ñ“]j
	if(ix!=0 || iy!=0)
	{
		// z²‚ÌŒX‚«‚Ì‹t’l
		rz = atan2(iy,ix) - PI2;
	}

// *this = rz * -ry * -rx
	SetZrotate(rz);
	Matrix tmp;
	if(ry)
	{
		tmp.SetYrotate(-ry);
		*this *= tmp;
	}
	if(rx)
	{
		tmp.SetXrotate(-rx);
		*this *= tmp;
	}

	int i,t;
	for( t=0 ; t<4 ; t++ )
		for( i=0 ; i<4 ; i++ )
			m[t][i] = NearZero(m[t][i]);
}

void
Matrix::SetOrientation(Vector &vz,Vector &vy)
{
	SetOrientation(vz.x, vz.y, vz.z,  vy.x, vy.y, vy.z);
}

Matrix&
Matrix::Invert()
{
	// ‹ts—ñ‚ğ‹‚ß‚éŒö®‚ğg‚¤‚ÆA“Á’èğŒ‚Å/0ƒGƒ‰[‚ğ‹N‚±‚·‚Ì‚ÅQuaternion‚ğŒo—R‚·‚éB
	Quaternion q;

	q = *this;

	q.Invert();

	*this = q;

	return *this;
}

void
Matrix::SetVector(float x,float y,float z)
{
	m[3][0] = (float)x;
	m[3][1] = (float)y;
	m[3][2] = (float)z;
}

// m = m(src)
Matrix&
Matrix::operator=(Matrix src)
{
	memcpy(m,src.m,sizeof(float)*16);

	return *this;
}

// m = m(src)
Matrix&
Matrix::operator=(Quaternion &src)
{
	float  wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
//	float* mat = m->f;

	x2 = src.b + src.b;		y2 = src.c + src.c;		z2 = src.d + src.d;
	xx = src.b * x2;		xy = src.b * y2;		xz = src.b * z2;
	yy = src.c * y2;		yz = src.c * z2;		zz = src.d * z2;
	wx = src.a * x2;		wy = src.a * y2;		wz = src.a * z2;

	m[0][0] = (float)(1.0 - (yy + zz));
	m[0][1] = (float)(xy + wz);
	m[0][2] = (float)(xz - wy);
	m[0][3] = .0f;

	m[1][0] = (float)(xy - wz);
	m[1][1] = (float)(1.0 - (xx + zz));
	m[1][2] = (float)(yz + wx);
	m[1][3] = .0f;

	m[2][0] = (float)(xz + wy);
	m[2][1] = (float)(yz - wx);
	m[2][2] = (float)(1.0 - (xx + yy));
	m[2][3] = .0f;

// b’èˆ’u
//	m[3][0] = .0f;
//	m[3][1] = .0f;
//	m[3][2] = .0f;
	m[3][3] = 1.0f;

	return *this;
}

Matrix&
Matrix::operator=(Vector &src)
{
	m[3][0] = src.x;
	m[3][1] = src.y;
	m[3][2] = src.z;

	return *this;
}

Matrix&
Matrix::operator+=(Vector &src)
{
	m[3][0] += src.x;
	m[3][1] += src.y;
	m[3][2] += src.z;

	return *this;
}

Matrix&
Matrix::operator-=(Vector &src)
{
	m[3][0] -= src.x;
	m[3][1] -= src.y;
	m[3][2] -= src.z;

	return *this;
}

// m *= m
Matrix&
Matrix::operator*=(Matrix &src)
{
	int t;
	float m2[4][4];


	memcpy(m2,m,sizeof(float)*16);

	// “WŠJ‚µ‚½•û‚ªˆ³“|“I‚É‘¬‚¢B
	for(t=0;t<4;t++)
	{
		m[t][0] = m2[t][0] * src.m[0][0]
				+ m2[t][1] * src.m[1][0]
				+ m2[t][2] * src.m[2][0]
				+ m2[t][3] * src.m[3][0];

		m[t][1] = m2[t][0] * src.m[0][1]
				+ m2[t][1] * src.m[1][1]
				+ m2[t][2] * src.m[2][1]
				+ m2[t][3] * src.m[3][1];

		m[t][2] = m2[t][0] * src.m[0][2]
				+ m2[t][1] * src.m[1][2]
				+ m2[t][2] * src.m[2][2]
				+ m2[t][3] * src.m[3][2];

		m[t][3] = m2[t][0] * src.m[0][3]
				+ m2[t][1] * src.m[1][3]
				+ m2[t][2] * src.m[2][3]
				+ m2[t][3] * src.m[3][3];
	}

	return *this;
}

// m' = m * m(src)
Matrix
Matrix::operator*(Matrix &src)
{
	int t;
	Matrix tmp;


	for(t=0;t<4;t++)
	{
		tmp.m[t][0] = m[t][0] * src.m[0][0]
					+ m[t][1] * src.m[1][0]
					+ m[t][2] * src.m[2][0]
					+ m[t][3] * src.m[3][0];

		tmp.m[t][1] = m[t][0] * src.m[0][1]
					+ m[t][1] * src.m[1][1]
					+ m[t][2] * src.m[2][1]
					+ m[t][3] * src.m[3][1];

		tmp.m[t][2] = m[t][0] * src.m[0][2]
					+ m[t][1] * src.m[1][2]
					+ m[t][2] * src.m[2][2]
					+ m[t][3] * src.m[3][2];

		tmp.m[t][3] = m[t][0] * src.m[0][3]
					+ m[t][1] * src.m[1][3]
					+ m[t][2] * src.m[2][3]
					+ m[t][3] * src.m[3][3];
	}

	return tmp;
}

Matrix
Matrix::operator* (Quaternion &src)
{
	Matrix tmp;

	tmp = src;
	tmp = *this * tmp;

	return tmp;
}

Matrix&
Matrix::operator*=(Quaternion &src)
{
	Matrix tmp;

	tmp = src;
	*this *= tmp;

	return *this;
}

Matrix&
Matrix::operator*=(float src)
{
	Matrix tmp;

	tmp.SetMagnification(src);
	*this *= tmp;

	return *this;
}

Matrix
Matrix::operator* (float src)
{
	Matrix tmp;

	tmp.SetMagnification(src);
	tmp = *this * tmp;

	return tmp;
}

Matrix&
Matrix::operator*=(Vector &src)
{
	Matrix tmp;

	tmp.SetMagnification(src);
	tmp = *this * tmp;

	return *this;
}

Matrix
Matrix::operator* (Vector &src)
{
	Matrix tmp;

	tmp.SetMagnification(src);
	tmp = *this * tmp;

	return tmp;
}

Matrix
Matrix::operator~()
{
	Matrix tmp;
	Quaternion q;

	q = *this;

	q.Invert();

	tmp = q;

	return tmp;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion()
{
	a = 1.0;
	b = c = d = .0;
}

Quaternion::~Quaternion()
{
}

Quaternion&
Quaternion::set(float ina,float inb,float inc,float ind)
{
	a = ina;
	b = inb;
	c = inc;
	d = ind;

	return *this;
}

Quaternion&
Quaternion::set(float inb,float inc,float ind)
{
	a = .0;
	b = inb;
	c = inc;
	d = ind;

	return *this;
}

void
Quaternion::put()
{
	printf("%11.6f,%11.6fi,%11.6fj,%11.6fk\n",a,b,c,d);
}

Quaternion&
Quaternion::Invert()
{
	b =- b;
	c =- c;
	d =- d;

	return *this;
}

void
Quaternion::SetXrotate(float r)
{
	SetRotate(1,0,0, r);
}
void
Quaternion::SetYrotate(float r)
{
	SetRotate(0,1,0, r);
}
void
Quaternion::SetZrotate(float r)
{
	SetRotate(0,0,1, r);
}

// q = q(src)
Quaternion&
Quaternion::operator=(Quaternion src)
{
	a = src.a;
	b = src.b;
	c = src.c;
	d = src.d;

	return *this;
}
//*
Quaternion&
Quaternion::operator=(Matrix &src)
{
	Vector vz(0,0,1), vy(0,1,0);

	vz *= src;
	vy *= src;

	SetOrientation(vz, vy);

	return *this;
}
//*/
/*
// “Á’èğŒ‰º‚Å³í“®ì‚µ‚È‚¢B
Quaternion&
Quaternion::operator=(Matrix &src)
{
	float  tr, s;
	float  q[4];
	int	i, j, k;
	int nxt[3] = {1, 2, 0};

	tr = src.m[0][0] + src.m[1][1] + src.m[2][2];

	// check the diagonal
	if (tr > 0.0)
	{
		s = (float)sqrt(tr + 1.0f);
		a = s / 2.0f;
		s = .5f / s;
		b = (src.m[1][2] - src.m[2][1]) * s;
		c = (src.m[2][0] - src.m[0][2]) * s;
		d = (src.m[0][1] - src.m[1][0]) * s;
	}
	else
	{ 
		// diagonal is negative
		i = 0;
		if (src.m[1][1] > src.m[0][0])
			i = 1;
		if (src.m[2][2] > src.m[i][i])
			i = 2;
		j = nxt[i];
		k = nxt[j];
		s = (float)sqrt((src.m[i][i] - (src.m[j][j] + src.m[k][k])) + 1.0f);
		q[i] = s * .5f;
		if (s != .0f)
			s = .5f / s;
		q[3] = (src.m[j][k] - src.m[k][j]) * s;
		q[j] = (src.m[i][j] + src.m[j][i]) * s;
		q[k] = (src.m[i][k] + src.m[k][i]) * s;

		a = q[0];
		b = q[1];
		c = q[2];
		d = q[3];
	}

	return *this;
}
//*/
/*
Quaternion&
Quaternion::operator=(Vector &src)
{
	a = 0;
	b = src.x;
	c = src.y;
	d = src.z;

	return *this;
}
*/
// q' = q + q(src)
Quaternion
Quaternion::operator+(Quaternion &src)
{
	Quaternion tmp;

	tmp.a = a + src.a;
	tmp.b = b + src.b;
	tmp.c = c + src.c;
	tmp.d = d + src.d;

	return tmp;
}

// q' = q * q(src)
Quaternion
Quaternion::operator*(Quaternion &src)
{
	Quaternion tmp;

	tmp.a = a*src.a - b*src.b - c*src.c - d*src.d;
	tmp.b = a*src.b + b*src.a + c*src.d - d*src.c;
	tmp.c = a*src.c + c*src.a + d*src.b - b*src.d;
	tmp.d = a*src.d + d*src.a + b*src.c - c*src.b;

	return tmp;
}

// q += q(src)
Quaternion&
Quaternion::operator+=(Quaternion &src)
{
	return *this = *this + src;
}

// q *= q(src)
Quaternion&
Quaternion::operator*=(Quaternion &src)
{
//	Quaternion tmp;
	float w=a, x=b, y=c, z=d;

	a = w*src.a - x*src.b - y*src.c - z*src.d;
	b = w*src.b + x*src.a + y*src.d - z*src.c;
	c = w*src.c + y*src.a + z*src.b - x*src.d;
	d = w*src.d + z*src.a + x*src.c - y*src.b;

	return *this;

//	return *this = *this * src;
}

Quaternion
Quaternion::operator* (Matrix &src)
{
	Quaternion tmp;

	tmp = src;
	tmp = *this * tmp;

	return tmp;
}

Quaternion&
Quaternion::operator*=(Matrix &src)
{
	Quaternion tmp;

	tmp = src;
	*this *= tmp;

	return *this;
}

// q' = q^-1  (‹t”)
Quaternion
Quaternion::operator~()
{
	Quaternion tmp;

	tmp.a = a;
	tmp.b =- b;
	tmp.c =- c;
	tmp.d =- d;

	return tmp;
}

// q' = q * float  (À”‚ÌæZ)
Quaternion
Quaternion::operator*(float src)
{
	Quaternion tmp;

	tmp.a = a * src;
	tmp.b = b * src;
	tmp.c = c * src;
	tmp.d = d * src;

	return tmp;
}

// q *= float  (À”‚ÌæZ)
Quaternion&
Quaternion::operator*=(float src)
{
	a *= src;
	b *= src;
	c *= src;
	d *= src;

	return *this;
}

Quaternion
Quaternion::operator/(float src)
{
	Quaternion tmp;

	tmp.a = a / src;
	tmp.b = b / src;
	tmp.c = c / src;
	tmp.d = d / src;

	return tmp;
}

// q *= float  (À”‚ÌæZ)
Quaternion&
Quaternion::operator/=(float src)
{
	a /= src;
	b /= src;
	c /= src;
	d /= src;

	return *this;
}

// ’·‚³‚P‚ÌƒxƒNƒgƒ‹‚SŒ³”¶¬
Quaternion&
Quaternion::SetRotate(float r)
{
	// Œö®ã r*2 ‚¾‚¯‰ñ“]‚µ‚Ä‚µ‚Ü‚¤‚½‚ß r ‚ğ”¼•ª‚É‚µ‚Ä‚â‚éB
	r /= 2;

	// a=0,sqrt(b*b+c*c+d*d)==1 ‚É‚·‚éB
	float f = sqrt(b*b + c*c + d*d);
	a = 0;
	b /= f;
	c /= f;
	d /= f;

	// ‚SŒ³”‚ğİ’è‚·‚é
	f = sin(r);
	a = cos(r);
	b = f * b;
	c = f * c;
	d = f * d;

	return *this;
}

Quaternion&
Quaternion::SetRotate(float inb,float inc,float ind,float r)
{
	set(inb, inc, ind);
	return SetRotate(r);
}

Quaternion&
Quaternion::SetRotate(Vector &v,float r)
{
	set(v.x, v.y, v.z);
	return SetRotate(r);
}

Quaternion&
Quaternion::SetRotate(Quaternion &q,float r)
{
	set(q.b, q.c, q.d);
	return SetRotate(r);
}

// q' = q * q(src) * q
Quaternion
Quaternion::CalcRotate(Quaternion &src)
{
	Quaternion tmp;

	tmp = *this * src;
//	tmp *= ~(*q1);
	tmp.a = a*src.a  - b*-src.b - c*-src.c - d*-src.d;
	tmp.b = a*-src.b + b*src.a  + c*-src.d - d*-src.c;
	tmp.c = a*-src.c + c*src.a  + d*-src.b - b*-src.d;
	tmp.d = a*-src.d + d*src.a  + b*-src.c - c*-src.b;

	return tmp;
}

// (0,0,1)‚ªA(ix¤iy¤iz)‚ğã(y‚ª³)‚É‚µ‚ÄA(x¤y¤z)‚ğŒü‚­Quaternion‚ğ¶¬
void
Quaternion::SetOrientation(float x,float y,float z,float ix,float iy,float iz)
{
// x,y‚Ì‡
	float rx=.0,ry=.0,rz=.0;
	float d,s,c;

// x²
	if(y!=0 || z!=0)
	{
		// x²‚ÌŒX‚«‚ğ“¾‚é
		rx = atan2(y,z);

		// x²‚Å“|‚·
		s = sin(rx);
		c = cos(rx);

		// y‚Í‚±‚ÌŒã•K—v–³‚¢‚Ì‚ÅŒvZ‚µ‚È‚¢
		z = NearZero(c*z + s*y);

		// p¨‰ñ“]
		d = iz;
		iz = NearZero(c*d  + s*iy);
		iy = NearZero(c*iy - s*d);
	}

// y²
	if(x!=0 || z!=0)
	{
		// y²‚ÌŒX‚«‚ğ“¾‚é
		ry = atan2(z,x) - PI2;

		// iz‚Í‚±‚ÌŒã•K—v–³‚¢‚Ì‚ÅŒvZ‚µ‚È‚¢
		ix = NearZero(cos(ry)*ix + sin(ry)*iz);
	}

// z²ip¨‰ñ“]j
	if(ix!=0 || iy!=0)
	{
		// z²‚ÌŒX‚«‚Ì‹t’l
		rz = atan2(iy,ix) - PI2;
	}

	Quaternion tmp;
	SetRotate(1,0,0, -rx);
	tmp.SetRotate(0,1,0, -ry);
	*this *= tmp;
	tmp.SetRotate(0,0,1, rz);
	*this *= tmp;

	a = NearZero(a);
	b = NearZero(b);
	c = NearZero(c);
	d = NearZero(d);
}

void
Quaternion::SetOrientation(Vector &vz,Vector &vy)
{
	SetOrientation(vz.x, vz.y, vz.z,  vy.x, vy.y, vy.z);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Vector::Vector()
{
    //	x = y = z = .0f;
    //‚±‚Ìˆ—‚·‚ç‚à—v‚ç‚È‚¢
}

Vector::Vector(float inx,float iny,float inz)
{
	x = (float)inx;
	y = (float)iny;
	z = (float)inz;
}

Vector::~Vector()
{
}

Vector&
Vector::set(float inx,float iny,float inz)
{
	x = (float)inx;
	y = (float)iny;
	z = (float)inz;

	return *this;
}

void
Vector::put()
{
	printf("%11.6f,%11.6f,%11.6f\n",x,y,z);
}

Vector&
Vector::Normal()
{
	float f = (float)sqrt(x*x + y*y + z*z);
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

float
Vector::Absolute()
{
	return sqrt(x*x + y*y + z*z);
}

float
Vector::Distance(Vector &src)
{
	return sqrt((x-src.x)*(x-src.x) + (y-src.y)*(y-src.y) + (z-src.z)*(z-src.z));

//	return (*this - src).Absolute();	// ’x‚¢
}

// “àÏ(‚QƒxƒNƒgƒ‹‚Ì¬‚·Šp“x)‚ğ•Ô‚·
float
Vector::InnerProduct(Vector &src)
{
	return acos((x*src.x + y*src.y + z*src.z) / sqrt((x*x + y*y + z*z)*(src.x*src.x + src.y*src.y + src.z*src.z)));
}

float
Vector::operator*(Vector &src)
{
	return (x*src.x + y*src.y + z*src.z);
}

// “àÏ(Ú(v1)(this)(v2)‚Ì¬‚·Šp“x)‚ğ•Ô‚·
float
Vector::InnerProduct(Vector &src1,Vector &src2)
{
	Vector v1 = src1-*this, v2 = src2-*this;

	return acos((v1.x*v2.x + v1.y*v2.y + v1.z*v2.z) / 
			sqrt((v1.x*v1.x + v1.y*v1.y + v1.z*v1.z)*(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z)));
}

// this, v1, v2 ‚Ì‡‚ÅŠOÏ‚ğ‹‚ßAthis‚ÉŠi”[‚·‚éB
Vector&
Vector::OuterProduct(Vector &v1,Vector &v2)
{
	Vector va = v1-*this, vb = v2-v1;

	x = va.y * vb.z - va.z * vb.y;
	y = va.z * vb.x - va.x * vb.z;
	z = va.x * vb.y - va.y * vb.x;

	return *this;
}

// this, v1, v2 ‚Ì‡‚ÅŠOÏ‚ğ‹‚ßAŒ‹‰Ê‚ÌVector‚ğ•Ô‚·B
Vector
Vector::CalcOuterProduct(Vector &v1,Vector &v2)
{
	Vector va = v1-*this, vb = v2-v1;
	Vector tmp;

	tmp.x = va.y * vb.z - va.z * vb.y;
	tmp.y = va.z * vb.x - va.x * vb.z;
	tmp.z = va.x * vb.y - va.y * vb.x;

	return tmp;
}

// returns v1 x v2
Vector CrossProduct(Vector &v1,Vector &v2)
{
	Vector tmp;

	tmp.x = v1.y * v2.z - v1.z * v2.y;
	tmp.y = v1.z * v2.x - v1.x * v2.z;
	tmp.z = v1.x * v2.y - v1.y * v2.x;

	return tmp;
}

// v = v(src)
Vector&
Vector::operator=(Vector src)
{
	x = src.x;
	y = src.y;
	z = src.z;

	return *this;
}

Vector
Vector::operator+(Vector &src)
{
	Vector tmp;

	tmp.x = x + src.x;
	tmp.y = y + src.y;
	tmp.z = z + src.z;

	return tmp;
}

Vector&
Vector::operator+=(Vector &src)
{
	x += src.x;
	y += src.y;
	z += src.z;

	return *this;
}

Vector
Vector::operator-(Vector &src)
{
    Vector tmp;
	tmp.x = x - src.x;
	tmp.y = y - src.y;
	tmp.z = z - src.z;

	return tmp;
}

Vector&
Vector::operator-=(Vector &src)
{
	x -= src.x;
	y -= src.y;
	z -= src.z;

	return *this;
}

Vector
Vector::operator*(float d)
{
	Vector tmp;

	tmp.x = (float)(x * d);
	tmp.y = (float)(y * d);
	tmp.z = (float)(z * d);

	return tmp;
}

Vector&
Vector::operator*=(float d)
{
	x *= (float)d;
	y *= (float)d;
	z *= (float)d;

	return *this;
}

Vector
Vector::operator/(float d)
{
	Vector tmp;

	tmp.x = (float)(x / d);
	tmp.y = (float)(y / d);
	tmp.z = (float)(z / d);

	return tmp;
}

Vector&
Vector::operator/=(float d)
{
	x /= (float)d;
	y /= (float)d;
	z /= (float)d;

	return *this;
}

// v *= m(src)
Vector&
Vector::operator*=(Matrix &src)
{
	float x2,y2,z2;


	x2 = x;
	y2 = y;
	z2 = z;

	x = src.m[0][0] * x2  +  src.m[1][0] * y2  +  src.m[2][0] * z2  +  src.m[3][0];
	y = src.m[0][1] * x2  +  src.m[1][1] * y2  +  src.m[2][1] * z2  +  src.m[3][1];
	z = src.m[0][2] * x2  +  src.m[1][2] * y2  +  src.m[2][2] * z2  +  src.m[3][2];

	return *this;
}

// v' = v * m(src)
Vector
Vector::operator*(Matrix &src)
{
	Vector tmp;

	tmp.x = src.m[0][0] * x  +  src.m[1][0] * y  +  src.m[2][0] * z  +  src.m[3][0];
	tmp.y = src.m[0][1] * x  +  src.m[1][1] * y  +  src.m[2][1] * z  +  src.m[3][1];
	tmp.z = src.m[0][2] * x  +  src.m[1][2] * y  +  src.m[2][2] * z  +  src.m[3][2];

	return tmp;
}

Vector&
Vector::operator=(Matrix &src)
{
	Vector tmp;

	x = src.m[3][0];
	y = src.m[3][1];
	z = src.m[3][2];

	return *this;
}

Vector&
Vector::operator=(Quaternion &src)
{
	x = (float)src.b;
	y = (float)src.c;
	z = (float)src.d;

	return *this;
}

// v' = q(src) * v * q(src)^-1
Vector
Vector::operator*(Quaternion &src)
{
	Vector tmp;
	float w1,x1,y1,z1;

	w1 =         - src.b*x - src.c*y - src.d*z;
	x1 = src.a*x           + src.c*z - src.d*y;
	y1 = src.a*y           + src.d*x - src.b*z;
	z1 = src.a*z           + src.b*y - src.c*x;

//	tmp.w = (float)(w1*src.a  - x1*-src.b - y1*-src.c - z1*-src.d);
	tmp.x = (float)(w1*-src.b + x1*src.a  + y1*-src.d - z1*-src.c);
	tmp.y = (float)(w1*-src.c + y1*src.a  + z1*-src.b - x1*-src.d);
	tmp.z = (float)(w1*-src.d + z1*src.a  + x1*-src.c - y1*-src.b);

	return tmp;
}

// v = q * v * q^-1
Vector&
Vector::operator*=(Quaternion &src)
{
	float w1,x1,y1,z1;

	w1 =         - src.b*x - src.c*y - src.d*z;
	x1 = src.a*x           + src.c*z - src.d*y;
	y1 = src.a*y           + src.d*x - src.b*z;
	z1 = src.a*z           + src.b*y - src.c*x;

//	w = (float)(w1*src.a  + x1*src.b + y1*src.c + z1*src.d);
	x = (float)(w1*-src.b + x1*src.a - y1*src.d + z1*src.c);
	y = (float)(w1*-src.c + y1*src.a - z1*src.b + x1*src.d);
	z = (float)(w1*-src.d + z1*src.a - x1*src.c + y1*src.b);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Hierarchy::Hierarchy(int inID)
{
	parent=prev=next=NULL;
	ID=inID;
	vnum=0;
	vlist=NULL;
}

Hierarchy::~Hierarchy()
{
	delete []vlist;
}

void
Hierarchy::Calc()
{
	Hierarchy *tmp=this;

	while(tmp)
	{
		tmp->DoCalc();
		tmp=tmp->next;
	}
}

HierarchyQ *
Hierarchy::AddChildQ()
{
	HierarchyQ *tmp;
	tmp = new HierarchyQ;

	tmp->parent = this;
	tmp->prev = this;
	tmp->next = next;

	if(next)
		next->prev=tmp;
	next=tmp;

	return tmp;
}

HierarchyM *
Hierarchy::AddChildM()
{
	HierarchyM *tmp;
	tmp = new HierarchyM;

	tmp->parent = this;
	tmp->prev = this;
	tmp->next = next;

	if(next)
		next->prev=tmp;
	next=tmp;

	return tmp;
}

Hierarchy *
Hierarchy::Search(int searchID)
{
	Hierarchy *tmp=this;

	while(tmp)
	{
		if(tmp->ID == searchID)
			return tmp;
		tmp=tmp->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HierarchyQ::HierarchyQ(int inID)
	:Hierarchy(inID)
{
}

HierarchyQ::~HierarchyQ()
{
}

void
HierarchyQ::DoCalc()
{
	if(parent)
	{
		parent->DoCalcChild(this);
	}
	else
	{
		sq = q;
		sv = v * sq;
	}
	if(vlist)
	{
		int i;
		for(i=0;i<vnum;i++)
		{
			
		}
	}
}

void
HierarchyQ::DoCalcChild(HierarchyQ *child)
{
	child->sq = sq * child->q;
	child->sv = child->v * child->sq + sv;
}

void
HierarchyQ::DoCalcChild(HierarchyM *child)
{
	// m=q*m
	Matrix tmp;
	tmp=q;
	child->sm = tmp * child->m;
	child->sv = child->v * child->sm + sv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HierarchyM::HierarchyM(int inID)
	:Hierarchy(inID)
{
}

HierarchyM::~HierarchyM()
{
}

void
HierarchyM::DoCalc()
{
	if(parent)
	{
		parent->DoCalcChild(this);
	}
	else
	{
		sm = m;
		sv = v * sm;
	}
}

void
HierarchyM::DoCalcChild(HierarchyM *child)
{
	child->sm = sm * child->m;
	child->sv = child->v * child->sm + sv;
}

void
HierarchyM::DoCalcChild(HierarchyQ *child)
{
	// q=m*q
	Quaternion tmp;
	tmp=m;
	child->sq = tmp * child->q;
	child->sv = child->v * child->sq + sv;
}

