
//-----------------------------------------------------------
//頂点クラス
//
//-----------------------------------------------------------
public class Vertex
{
	Vector v = new Vector();//頂点座標
	Vector v_;
	Vector n = new Vector();//法線ベクトル
	
	float tu;				//テクスチャ座標
	float tv;
	
	int used;	//使用回数
	
	Vertex()
	{
		n.x = 0;
		n.y = 0;
		n.z = 0;
		used = 0;
	}
	
	Vertex(float x,float y,float z)
	{
		v.x = x;
		v.y = y;
		v.z = z;
		n.x = 0;
		n.y = 0;
		n.z = 0;
		used = 0;
	}
	
	Vertex(float x,float y,float z,float nx,float ny,float nz)
	{
		v.x = x;
		v.y = y;
		v.z = z;
		n.x = nx;
		n.y = ny;
		n.z = nz;
		used = 0;
	}
	
	Vertex(float x,float y,float z,float nx,float ny,float nz,float _u,float _v)
	{
		v.x = x;
		v.y = y;
		v.z = z;
		n.x = nx;
		n.y = ny;
		n.z = nz;
		tu = _u;
		tv = _v;
		used = 0;
	}
}
