/*
	Geometry Class Library
					Ver1.00 Release 000
					Wild Anarchy YU-kA (kouiti@lt.sakura.ne.jp)


以下のクラスから構成される。
	行列演算(Matrix)
	４元数演算(Quaternion)
	ベクトル演算(Vector)

これらのクラスは、ゲームで使用することを目的に設計しているため、
ほとんどの関数は展開されている。
たとえば、+= 演算子は通常 = と + を組み合わせて生成するが、
これらのクラスでは極力展開し高速化している。（特に良く使うと思われる部分等）
ソースコードが肥大化して汚いが速度を稼ぐためである。
（それでもまだまだ高速化の余地は残っているとは思う）

これらのクラスは、互いに演算することが出来る。

	行列同士の計算
		Matrix = Matrix * Matrix
		Matrix *= Matrix

	行列による座標変換
		Vector = Vector * Matrix

	４元数同士の計算
		Quaternion = Quaternion * Quaternion
		Quaternion *= Quaternion
		Quaternion = Quaternion + Quaternion
		Quaternion += Quaternion
		~Quaternion		(Quaternion^-1 , Quaternionの逆数)
	ただし、４元数同士の乗算は、行列の乗算の逆になる点に注意。
	つまり、
		(m1 == q1)
		(m2 == q2)
	の時、
		m = m1 * m2;
		q = q2 * q1;
	とした時に
		(m == q)
	となる。


	４元数による座標変換(回転)
		Quaternion = Quaternion * VectorQuaternion * Quaternion^-1
		Vector = Quaternion * Vector * Quaternion^-1

これらの演算を演算子（または関数）で行える。


行列は、m1 , m2 の順に座標変換したい場合、m1 * m2 とするが、
４元数は、q1 , q2 の順に座標変換したい場合 q2 * q1 と逆になる点に注意。
（これはバグではなく、４元数の特性のようだ）

Geometry.h を呼ぶ前に、d3dtypes.h を呼ぶとVectorクラスを
LPD3DVECTORのかわりに渡せる。
同様にLPD3DMATRIXにはMatrixクラスを渡すことが出来る。
また、D3DRMMATRIX4D(double [4][4])に対しては、Matrix::mを
直接渡せば良い。


以下の関数(operator=)は、Nick Bobic氏のアルゴリズムを使用。
	行列から４元数へ変換
		Quaternion&	operator=(Matrix &src);


参考文献
	「Ｃ言語による最新アルゴリズム事典」奥村 晴彦 著（技術評論社）
		この手のアルゴリズム本の中では最優良本である、と思う。
		実用的なアルゴリズムが多数掲載されている。
		ただし、読み手にそれなりの知識が要求される。
		C/C++プログラマなら、Ｋ＆Ｒと一緒に１冊持っていて損は無い。
		（むしろ私はＫ＆Ｒよりは、こっちの方が重要だと確信している）

	「超複素数入門」И．Л．Кантор、А．С．Солодовников著
					浅野 洋 監訳・笠原 久弘 訳（森北出版株式会社）
		４元数に間する知識が皆無だった私が読んで理解できるくらいわかりやすい
		解説がされている。
		４元数の解説されているたった２１ページしか読んでないが、
		２６００円は良い買い物だった。
		（１ページも使い物にならない本も多数存在する）


転載、改変再配布について
	GPL準拠でご使用頂ければ、問題無いです。
	また、アセンブラ等で最適化された方がいらっしゃいましたら、
	私にもください。（笑
	出来れば、E-3DNow希望です。 ＜あつかましい


履歴
00/05/29	Matrix::SetOrientation()にバグ。修正。 > Ver1.00β01
00/05/30	Matrix に =Quaternion追加 > Ver1.00β02
00/06/23	細部を修正
			Matrixから移動ベクトルをVectorにコピー、加減算するoperator追加。
			SetOrientation()のVectorの引数をポインタからアドレス演算子に変更。
			同じくSetRotate()のVectorの引数も修正。
			Matrix::Invert()を追加。（逆行列を求める）
00/06/24	Quaternion&	operator=(Matrix &src);にバグがあったため修正
			Quaternion::SetOrientation(),Matrix::SetOrientation()のバグ修正。
			Matrix::Invert()が特定条件下で動作しなかったため修正。
00/06/25	Matrix::SetMagnification()追加
00/06/27	Quaternion * Matrix , Matrix * Quaternion の式が有効になった。
00/06/27	Quaternion::Quaternion&	operator= (Vector &src) を削除。
			理由はMatrixの場合と動作が異なってしまうため。
00/07/03	Vectorクラスに内積、外積の演算関数を追加。
00/07/05	細部を修正

*/

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

//#include <float.h>

#ifndef PI
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define PI			M_PI
#define PI2			M_PI_2
#endif

//#define float double

// 0に近い値を0に丸める。
#define DBL_EPSILON 2.2204460492503131E-16
#define NearZero(d)	((d<DBL_EPSILON && d>-DBL_EPSILON) ? 0 : d)
#define IsZero(d)	((d<DBL_EPSILON && d>-DBL_EPSILON) ? 1 : 0)

class Vector;
class Quaternion;

class Matrix
{
public:
					Matrix();
	virtual			~Matrix();

	float			m[4][4];

	void			put();

// X,Y,Z、それぞれの軸に対して、rだけ回転する行列を生成
	void			SetXrotate(float r);
	void			SetYrotate(float r);
	void			SetZrotate(float r);
// X,Y,Z、それぞれの軸に対して、rだけ回転する行列を生成
	void			SetXrotate(float s,float c);
	void			SetYrotate(float s,float c);
	void			SetZrotate(float s,float c);
// 'F'はfirst,fastの意。thisが初期化されて
// いる場合（コンストラクト直後等）のみ有効
// （最初だけ(first)使用でき、高速(fast)）
	void			SetXrotateF(float r);
	void			SetYrotateF(float r);
	void			SetZrotateF(float r);

// 拡大縮小する行列生成
	void			SetMagnification(float src);
	void			SetMagnification(float x,float y,float z);
	void			SetMagnification(Vector &src);

// zベクトル成分が(x,y,z)を向き、yベクトル成分が(ix,iy,iz)を向く変換を行う行列を生成
	void			SetOrientation(float x,float y,float z,float ix,float iy,float iz);
	void			SetOrientation(Vector &vz,Vector &vy);

	Matrix&			Invert();
// 移動ベクトルセット
	void			SetVector(float x=0,float y=0,float z=0);

	float&			operator()(int y, int x) { return m[y][x]; };
	Matrix&			operator= (Matrix src);
// ４元数から行列へ変換
	Matrix&			operator= (Quaternion &src);
// 移動座標を代入、加減算(m[3][0],m[3][1],m[3][2]だけを書き換え)
	Matrix&			operator= (Vector &src);
	Matrix&			operator+=(Vector &src);
	Matrix&			operator-=(Vector &src);
// 行列の乗算
	Matrix&			operator*=(Matrix &src);
	Matrix			operator* (Matrix &src);
// 行列 * ４元数
	Matrix			operator* (Quaternion &src);
	Matrix&			operator*=(Quaternion &src);

// (x,y,z)を一律src倍する行列にする
	Matrix&			operator*=(float src);
	Matrix			operator* (float src);
// 各方向をsrc倍する行列にする
	Matrix&			operator*=(Vector &src);
	Matrix			operator* (Vector &src);
// 逆行列
	Matrix			operator~();

#ifdef _D3DTYPES_H_
	operator D3DMATRIX*() {return (D3DMATRIX*)m;};
#endif
};

class Quaternion
{
public:
					Quaternion();
	virtual			~Quaternion();

// q = a + bi + cj + dk (b,c,dはそれぞれx,y,zに対応する)
	float			a,b,c,d;

// 値を代入
	Quaternion&		set(float ina,float inb,float inc,float ind);
// a は 0 になる。
	Quaternion&		set(float inb,float inc,float ind);
// デバッグ用
	void			put();
// ４元数の逆数
	Quaternion&		Invert();

	void			SetXrotate(float r);
	void			SetYrotate(float r);
	void			SetZrotate(float r);

	Quaternion&		operator= (Quaternion src);
// 行列から４元数へ変換
	Quaternion&		operator= (Matrix &src);
// ベクトルのコピー。実数部は 0 で初期化される
//	Quaternion&		operator= (Vector &src);
// ４元数の加算
	Quaternion		operator+ (Quaternion &src);
	Quaternion&		operator+=(Quaternion &src);
// ４元数の乗算
	Quaternion		operator* (Quaternion &src);
	Quaternion&		operator*=(Quaternion &src);
// ４元数 * 行列
	Quaternion		operator* (Matrix &src);
	Quaternion&		operator*=(Matrix &src);

// ４元数の逆数
	Quaternion		operator~();
// 実数との乗除算
	Quaternion		operator* (float src);
	Quaternion&		operator*=(float src);
	Quaternion		operator/ (float src);
	Quaternion&		operator/=(float src);
// 実数との加算は、実数部である a に加算するだけなので省略（しかも使わない）

// *this の b,c,d をベクトルとして、その回りを r だけ回転する４元数を作る。
// 回転は、左手座標系のとき、ベクトルに対して左回り（反時計回り）である。
// また、SetRotate()を呼んでもベクトル向きは変わらないが、特定条件下で呼んだ場合
// (b=c=d=0の時等) 正常動作しないため基本的には毎回ベクトル値を入れなおす。
//	Quaternion&		SetRotate(float r);
// ベクトル値も同時にセットする。
	Quaternion&		SetRotate(float inb,float inc,float ind,float r);
	Quaternion&		SetRotate(Vector &v,float r);
	Quaternion&		SetRotate(Quaternion &q,float r);
// SetRotate() 後に呼ぶと、src を回転させたベクトル４元数を返す。
	Quaternion		CalcRotate(Quaternion &src);

// Matrixと互換。詳しくはMatrix::SetOrientation()参照
	void			SetOrientation(float x,float y,float z,float ix,float iy,float iz);
	void			SetOrientation(Vector &vz,Vector &vy);

protected:
	Quaternion&		SetRotate(float r);
	Quaternion&		operator= (Vector &src);
};

class Vector
{
public:
					Vector();
					Vector(float inx,float iny,float inz);
	virtual			~Vector();

	float			x,y,z;

	Vector&			set(float inx=0,float iny=0,float inz=0);
// デバッグ用
	void			put();

// 法線にする。 x*x + y*y + z*z == 1 になる。
	Vector&			Normal();
// 絶対値を求める。sqrt(x*x + y*y + z*z)を返す
	float			Absolute();
// 2点間の距離を求める
	float			Distance(Vector &src);

// 内積(２ベクトルの成す角度)を返す
	float			InnerProduct(Vector &src);
	float			operator*(Vector &src);
// 内積(∠(v1)(this)(v2)の成す角度)を返す
	float			InnerProduct(Vector &src1,Vector &src2);
// this, v1, v2 の順で外積を求め、thisに格納する。
	Vector&			OuterProduct(Vector &v1,Vector &v2);
// this, v1, v2 の順で外積を求め、結果のVectorを返す。
	Vector			CalcOuterProduct(Vector &v1,Vector &v2);

	Vector&			operator= (Vector src);
// ベクトル同士の加減算
	Vector			operator+ (Vector &src);
	Vector&			operator+=(Vector &src);
	Vector			operator-(Vector &src);
	Vector&			operator-=(Vector &src);
// ベクトルと実数の乗除算
	Vector			operator* (float d);
	Vector&			operator*=(float d);
	Vector			operator/ (float d);
	Vector&			operator/=(float d);

// 座標変換。単純に行列演算を行うだけ
	Vector&			operator*=(Matrix &src);
	Vector			operator* (Matrix &src);
// 移動ベクトルをコピー
	Vector&			operator= (Matrix &src);

// ベクトルのコピー。実数部は切り捨て
	Vector&			operator= (Quaternion &src);

// 座標変換。４元数の掛算とは全くの別物。実際は q * v * ~q を行っている。
// ＊＊＊要注意
	Vector			operator* (Quaternion &src);
	Vector&			operator*=(Quaternion &src);

#ifdef _D3DTYPES_H_
	Vector&			operator= (D3DVECTOR src){x=src.x; y=src.y; z=src.z; return *this;};
	operator D3DVECTOR*() {return (D3DVECTOR*)this;};
#endif
};


// 行列、４元数混在の階層クラスのテストモデル
// 実際は４元数から行列への変換は高速に行え、
// 行列で出きることはたいてい４元数で出来、
// 更に4元数同士の乗算は高速なため
// 内部実装はすべて Quaternion + Vector にして、
// 大量の頂点の座標演算を行う場合だけ行列と頂点の乗算を行うようにした方が良い。

class HierarchyQ;
class HierarchyM;
class Hierarchy
{
public:
friend class HierarchyQ;
friend class HierarchyM;
						Hierarchy(int inID=0);
	virtual				~Hierarchy();

	int					ID;

	Vector				v;		// 自身のベクトル
	Vector				sv;		// 親からの全てを加えた座標

	int					vnum;	// 頂点数
	Vector				*vlist;	// 頂点の配列

	virtual void		Calc();

	virtual Hierarchy	*Search(int searchID);

// 子供を追加
	virtual HierarchyQ	*AddChildQ();
	virtual HierarchyM	*AddChildM();


// 基本的に一次元リストである。
// 自分は子供を知らない。そのかわり親を知っているので座標変換が行える。
// また、親はリストの中で自分より前にいなければならない。
// つまり、相互に親として指している構造はエラーである。
	Hierarchy			*parent,*prev,*next;

protected:
	virtual void		DoCalc(){};
	virtual void		DoCalcChild(HierarchyQ *child){};
	virtual void		DoCalcChild(HierarchyM *child){};
};

class HierarchyQ : public Hierarchy
{
public:
						HierarchyQ(int inID=0);
						~HierarchyQ();
	Quaternion			q;		// 自身の変換
	Quaternion			sq;		// 親からの全ての変換をした４元数
protected:
	void				DoCalc();
	void				DoCalcChild(HierarchyQ *child);
	void				DoCalcChild(HierarchyM *child);
};

class HierarchyM : public Hierarchy
{
public:
						HierarchyM(int inID=0);
						~HierarchyM();
	Matrix				m;		// 自身の変換
	Matrix				sm;		// 親からの全ての変換をした行列
protected:
	void				DoCalc();
	void				DoCalcChild(HierarchyM *child);
	void				DoCalcChild(HierarchyQ *child);
};


//独立関数
Vector CrossProduct(Vector &v1,Vector &v2);

#endif

/*
// Geometry サンプル
int main()
{
// ４元数サンプル
	Quaternion q;		// 軸
	Vector v;			// 回転させる座標点

// (1,1,0)のベクトルに対して、ベクトル v を左回りに90度回転させる。
	q.set(1,1,0);		// 軸にするベクトルを設定
	q.SetRotate(PI2);	// 回転する角度を設定
	v.set(1,0,0);		// 回転させるベクトルを設定
	v*=q;				// ベクトルを回転させる
	v.put();			// 結果表示

// 同じく、(1,1,0)のベクトルに対して、ベクトル v を左回りに180度回転させる。
	q.set(1,1,0);
	q.SetRotate(PI);	// 回転する角度を再設定
	v.set(1,0,0);
	v*=q;
	v.put();


// 行列演算サンプル
	Matrix m,m2;

	m.SetYrotate(PI2);	// (z,x)平面に対して、PI2だけ回転する行列
	v.set(1,0,0);
	v*=m;				// 行列とベクトル（広い意味でこれも行列）の乗算
	v.put();

	m2.SetXrotate(PI2);	// (y,z)平面に対して、PI2だけ回転する行列
	m*=m2;				// 行列の乗算
	v.set(1,0,0);
	v*=m;
	v.put();

// 行列 ⇔ ４元数 サンプル

	m=q;				// qと完全に同じ変換を行うmを生成。
	v.set(1,1,1);
	v*=q;
	v.put();			// …①
	v.set(1,1,1);
	v*=m;
	v.put();			// ①と同じ結果が得られる。

	m.SetOrientation(1,2,3, 1,1.5,1.2);
	q=m;				// mと完全に同じ変換を行うqを生成。
	v.set(1,1,1);
	v*=q;
	v.put();			// …②
	v.set(1,1,1);
	v*=m;
	v.put();			// ②と同じ結果が得られる。


	return 0;
}
*/
