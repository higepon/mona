
//-----------------------------------------------------------
//マトリクスクラス
//
//-----------------------------------------------------------
public class Matrix
{
	//-----------------------------------------------------------
	//メンバ変数
	//
	//-----------------------------------------------------------
	float m00,m01,m02,m03;
	float m10,m11,m12,m13;
	float m20,m21,m22,m23;
	float m30,m31,m32,m33;
	
	//-----------------------------------------------------------
	//単位行列で初期化
	//
	//-----------------------------------------------------------
	void Initialize()
	{
		//|  1  0  0  0|
		//|  0  1  0  0|
		//|  0  0  1  0|
		//|  0  0  0  1|

		m00 = 1;m01 = 0;m02 = 0;m03 = 0;
		m10 = 0;m11 = 1;m12 = 0;m13 = 0;
		m20 = 0;m21 = 0;m22 = 1;m23 = 0;
		m30 = 0;m31 = 0;m32 = 0;m33 = 1;
	}

	//-----------------------------------------------------------
	//初期化
	//
	//-----------------------------------------------------------
	void Initialize(Matrix m)
	{
		m00 = m.m00;m01 = m.m01;m02 = m.m02;m03 = m.m03;
		m10 = m.m10;m11 = m.m11;m12 = m.m12;m13 = m.m13;
		m20 = m.m20;m21 = m.m21;m22 = m.m22;m23 = m.m23;
		m30 = m.m30;m31 = m.m31;m32 = m.m32;m33 = m.m33;
	}

	//-----------------------------------------------------------
	//行列の掛け算（合成）
	//
	//Matrix m ...合成するマトリクス
	//-----------------------------------------------------------
	void Mult(Matrix m)
	{
		float _00 = m00*m.m00 + m01*m.m10 + m02*m.m20 + m03*m.m30;
		float _01 = m00*m.m01 + m01*m.m11 + m02*m.m21 + m03*m.m31;
		float _02 = m00*m.m02 + m01*m.m12 + m02*m.m22 + m03*m.m32;
		float _03 = m00*m.m03 + m01*m.m13 + m02*m.m23 + m03*m.m33;
		float _10 = m10*m.m00 + m11*m.m10 + m12*m.m20 + m13*m.m30;
		float _11 = m10*m.m01 + m11*m.m11 + m12*m.m21 + m13*m.m31;
		float _12 = m10*m.m02 + m11*m.m12 + m12*m.m22 + m13*m.m32;
		float _13 = m10*m.m03 + m11*m.m13 + m12*m.m23 + m13*m.m33;
		float _20 = m20*m.m00 + m21*m.m10 + m22*m.m20 + m23*m.m30;
		float _21 = m20*m.m01 + m21*m.m11 + m22*m.m21 + m23*m.m31;
		float _22 = m20*m.m02 + m21*m.m12 + m22*m.m22 + m23*m.m32;
		float _23 = m20*m.m03 + m21*m.m13 + m22*m.m23 + m23*m.m33;
		float _30 = m30*m.m00 + m31*m.m10 + m32*m.m20 + m33*m.m30;
		float _31 = m30*m.m01 + m31*m.m11 + m32*m.m21 + m33*m.m31;
		float _32 = m30*m.m02 + m31*m.m12 + m32*m.m22 + m33*m.m32;
		float _33 = m30*m.m03 + m31*m.m13 + m32*m.m23 + m33*m.m33;
		
		m00 = _00; m01 = _01;m02 = _02;m03 = _03;
		m10 = _10; m11 = _11;m12 = _12;m13 = _13;
		m20 = _20; m21 = _21;m22 = _22;m23 = _23;
		m30 = _30; m31 = _31;m32 = _32;m33 = _33;
	}

	//-----------------------------------------------------------
	//拡大縮小
	//
	//float sx ...Ｘ倍率
	//float sy ...Ｙ倍率
	//float sz ...Ｚ倍率
	//-----------------------------------------------------------
	void Scale(float sx,float sy,float sz)
	{		
		//| sx  0  0  0|
		//|  0 sy  0  0|
		//|  0  0 sz  0|
		//|  0  0  0  1|

		Matrix m = new Matrix();
		m.m00 = sx;m.m01 =  0;m.m02 =  0;m.m03 = 0;
		m.m10 =  0;m.m11 = sy;m.m12 =  0;m.m13 = 0;
		m.m20 =  0;m.m21 =  0;m.m22 = sz;m.m23 = 0;
		m.m30 =  0;m.m31 =  0;m.m32 =  0;m.m33 = 1;

		//行列の合成
		this.Mult(m);
	}
	
	//-----------------------------------------------------------
	//Ｘ軸回転
	//
	//float r ...回転角（単位ラジアン）
	//-----------------------------------------------------------
	void RotateX(float r)
	{
		float sinx = (float)Math.sin(r);
		float cosx = (float)Math.cos(r);	

		//X軸回転
		//| 1     0    0 0|
		//| 0  cosX sinX 0|
		//| 0 -sinX cosX 0|
		//| 0     0    0 1|	

		Matrix m = new Matrix();
		m.m00 = 1;m.m01 =    0;m.m02 =    0;m.m03 = 0;
		m.m10 = 0;m.m11 = cosx;m.m12 = sinx;m.m13 = 0;
		m.m20 = 0;m.m21 =-sinx;m.m22 = cosx;m.m23 = 0;
		m.m30 = 0;m.m31 =    0;m.m32 =    0;m.m33 = 1;

		//行列の合成
		this.Mult(m);
	}
	
	//-----------------------------------------------------------
	//Ｙ軸回転
	//
	//float r ...回転角（単位ラジアン）
	//-----------------------------------------------------------
	void RotateY(float r)
	{
		float siny = (float)Math.sin(r);
		float cosy = (float)Math.cos(r);

		//Y軸回転
		//| cosY 0 -sinY 0|
		//|    0 1     0 0|
		//| sinY 0  cosY 0|
		//|    0 0     0 1|

		Matrix m = new Matrix();
		m.m00 = cosy;m.m01 = 0;m.m02 =-siny;m.m03 = 0;
		m.m10 =    0;m.m11 = 1;m.m12 =    0;m.m13 = 0;
		m.m20 = siny;m.m21 = 0;m.m22 = cosy;m.m23 = 0;
		m.m30 =    0;m.m31 = 0;m.m32 =    0;m.m33 = 1;

		//行列の合成
		this.Mult(m);
	}

	//-----------------------------------------------------------
	//Ｚ軸回転	
	//
	//float r ...回転角（単位ラジアン）
	//-----------------------------------------------------------
	void RotateZ(float r)
	{
		float sinz = (float)Math.sin(r);
		float cosz = (float)Math.cos(r);	

		//Z軸回転
		//|  cosZ sinZ 0 0|
		//| -sinZ cosZ 0 0|
		//|     0    0 1 0|
		//|     0    0 0 1|	

		Matrix m = new Matrix();
		m.m00 = cosz;m.m01 = sinz;m.m02 = 0;m.m03 = 0;
		m.m10 =-sinz;m.m11 = cosz;m.m12 = 0;m.m13 = 0;
		m.m20 =    0;m.m21 =    0;m.m22 = 1;m.m23 = 0;
		m.m30 =    0;m.m31 =    0;m.m32 = 0;m.m33 = 1;

		//行列の合成
		this.Mult(m);
	}

	//-----------------------------------------------------------
	//平行移動
	//
	//float dx ...Ｘ移動量
	//float dy ...Ｙ移動量
	//float dz ...Ｚ移動量
	//-----------------------------------------------------------
	void Translate(float dx,float dy,float dz)
	{
		//|  1  0  0  0|
		//|  0  1  0  0|
		//|  0  0  1  0|
		//| dx dy dz  1|

		Matrix m = new Matrix();
		m.m00 =  1;m.m01 =  0;m.m02 =  0;m.m03 = 0;
		m.m10 =  0;m.m11 =  1;m.m12 =  0;m.m13 = 0;
		m.m20 =  0;m.m21 =  0;m.m22 =  1;m.m23 = 0;
		m.m30 = dx;m.m31 = dy;m.m32 = dz;m.m33 = 1;

		//行列の合成
		this.Mult(m);	
	}

	//-----------------------------------------------------------
	//一次変換
	//
	//Vector a ...変換する座標
	//
	//戻値：変換された座標が返る。
	//-----------------------------------------------------------
	Vector Transform(Vector a)
	{
		Vector v = new Vector();
		v.x = m00 * a.x + m10 * a.y + m20 * a.z + m30;
		v.y = m01 * a.x + m11 * a.y + m21 * a.z + m31;
		v.z = m02 * a.x + m12 * a.y + m22 * a.z + m32;

		return v;
	}
	
	//-----------------------------------------------------------
	//移動無視の一次変換
	//
	//Vector a ...変換する座標
	//
	//戻値：変換された座標が返る。
	//-----------------------------------------------------------
	Vector Rotation(Vector a)
	{
		Vector v = new Vector();
		v.x = m00 * a.x + m10 * a.y + m20;
		v.y = m01 * a.x + m11 * a.y + m21;
		v.z = m02 * a.x + m12 * a.y + m22;

		return v;
	}

	//-----------------------------------------------------------
	//転置行列の作成
	//
	//-----------------------------------------------------------
	void Invert(Matrix v)
	{
		m00=v.m00;m01=v.m10;m02=v.m20;m03=0;
		m10=v.m01;m11=v.m11;m12=v.m21;m13=0;
		m20=v.m02;m21=v.m12;m22=v.m22;m23=0;
		m30=    0;m31=    0;m32=    0;m33=1f;		
	}
	
	//-----------------------------------------------------------
	//ビュー変換行列の作成
	//
	//Vector from ...カメラの位置
	//Vector at   ...ターゲットの位置
	//Vector wup  ...カメラの上方向
	//-----------------------------------------------------------
	void View(Vector from,Vector at,Vector wup)
	{
		Vector view = new Vector();
		view.x = at.x - from.x;
		view.y = at.y - from.y;
		view.z = at.z - from.z;
		
		view.Normalize();
		
		float dot = wup.DotProduct(view);
		
		Vector up = new Vector();
		
		up.x = wup.x - dot * view.x;
		up.y = wup.y - dot * view.y;
		up.z = wup.z - dot * view.z;
		
		up.Normalize();
		
		Vector right = new Vector();
		right.CrossProduct(up,view);
		
		m00 = right.x;	m01 = up.x;	m02 = view.x;	m03 = 0;
		m10 = right.y;	m11 = up.y;	m12 = view.y;	m13 = 0;
		m20 = right.z;	m21 = up.z;	m22 = view.z;	m23 = 0;
		m30 = -from.DotProduct(right);
		m31 = -from.DotProduct(up   );
		m32 = -from.DotProduct(view );
		m33 =  1.0f;
	}
	
	//-----------------------------------------------------------
	//射影変換行列
	//
	//-----------------------------------------------------------
	void Projection(float fov,float aspect)
	{
		float w = aspect * (float)Math.cos(fov * 0.5f)/(float)Math.sin(fov * 0.5f);
		float h = 1.0f   * (float)Math.cos(fov * 0.5f)/(float)Math.sin(fov * 0.5f);
		
		Initialize();
		
		m00 = w;
		m11 = h;
		m22 = 1;
		m23 = 1;
		m33 = 0;
	}
	
	//-----------------------------------------------------------
	//射影変換行列
	//
	//-----------------------------------------------------------
	void Projection(float near,float far,float fov,float aspect)
	{
		float w = aspect * (float)(Math.cos(fov * 0.5f) / Math.sin(fov * 0.5f));
		float h = 1.0f   * (float)(Math.cos(fov * 0.5f) / Math.sin(fov * 0.5f));
		float q = far / (far - near);
		
		Initialize();
		
		m00 = w;
		m11 = h;
		m22 = q;
		m23 = 1.0f;
		m32 = -q * near;
	}

	//-----------------------------------------------------------
	//クリップ行列
	//
	//-----------------------------------------------------------
	void Clip(float cx,float cy,float cw,float ch,float minz,float maxz)
	{
		Initialize();
		m00 = 1/cw;
		m11 = 1/ch;
		m22 = 1/(maxz-minz);
		m30 = -cx/cw;
		m31 = -cy/ch;
		m32 = -minz/(maxz-minz);
	}

	//-----------------------------------------------------------
	//ビューポート行列
	//
	//-----------------------------------------------------------
	void ViewPort(float x,float y,float w,float h)
	{
		Initialize();
		m00 =  w;
		m11 =  h;
		m30 =  x;
		m31 = -y;
	}
	
	//-----------------------------------------------------------
	//ジオメトリブレンド
	//
	//-----------------------------------------------------------
	void Blend(Matrix m1,Matrix m2,float rate)
	{
		m00 = (m1.m00 - m2.m00) * rate + m2.m00; 
		m01 = (m1.m01 - m2.m01) * rate + m2.m01; 
		m02 = (m1.m02 - m2.m02) * rate + m2.m02; 
		m03 = (m1.m03 - m2.m03) * rate + m2.m03; 
		m10 = (m1.m10 - m2.m10) * rate + m2.m10; 
		m11 = (m1.m11 - m2.m11) * rate + m2.m11; 
		m12 = (m1.m12 - m2.m12) * rate + m2.m12; 
		m13 = (m1.m13 - m2.m13) * rate + m2.m13; 
		m20 = (m1.m20 - m2.m20) * rate + m2.m20; 
		m21 = (m1.m21 - m2.m21) * rate + m2.m21; 
		m22 = (m1.m22 - m2.m22) * rate + m2.m22; 
		m23 = (m1.m23 - m2.m23) * rate + m2.m23; 
		m30 = (m1.m30 - m2.m30) * rate + m2.m30; 
		m31 = (m1.m31 - m2.m31) * rate + m2.m31; 
		m32 = (m1.m32 - m2.m32) * rate + m2.m32; 
		m33 = (m1.m33 - m2.m33) * rate + m2.m33; 
	}
	
	//-----------------------------------------------------------
	//平行光源が平面につくる影
	//
	//-----------------------------------------------------------
	void Shadow(Vector L)
	{
		//|     1    0     0    0 |
		//| -L.x/L.y 0 -L.z/L.y 0 |
		//|     0    0     1    0 |
		//|     0    0     0    1 |
		m00 =       1;m01 =  0;m02 =       0;m03 = 0;
		m10 =-L.x/L.y;m11 =  0;m12 =-L.z/L.y;m13 = 0;
		m20 =       0;m21 =  0;m22 =       1;m23 = 0;
		m30 =       0;m31 =  0;m32 =       0;m33 = 1;
	}
}
