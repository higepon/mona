
//------------------------------------------------------
//シンプルな図形を作成する。
//
//------------------------------------------------------
public class SimpleShape
{
	VERTEX vertices[];
	short  indices [];
	
	static public float min;
	static public float max;
	
	//球
	public void CreateSphere(int n)
	{
		float c  = .001f;
		float ac = (float)Math.PI/n - .001f;
		int   p  = 0;
		
		//頂点データの作成
		vertices = new VERTEX[n * n + n];
		for (int i=0;i<n+1;i++,c+=ac){
			float s = (float)Math.sin(c);
			float z = (float)Math.cos(c);
			float a = 0;
			for (int j=0;j<n;j++,a+= Math.PI/n*2){
				//頂点座標設定
				float x = (float)(Math.cos(a)) * s;
				float y = (float)(Math.sin(a)) * s;
				vertices[p] = new VERTEX();
				vertices[p].v.x = x;
				vertices[p].v.y = y;
				vertices[p].v.z = z;
			
				//UV座標設定（球面ラップ)
				vertices[p].tu = (float)(Math.atan2(z,           x        ) / (Math.PI*2) + .5);
				vertices[p].tv = (float)(Math.atan2(y,Math.sqrt(x*x + z*z)) / (Math.PI  ) + .5);
				p++;
			}
		}
		
		//インデックスデータの作成
		indices = new short[n * n * 4];
		p = 0;
		for (int j=0;j<n;j++){
			int off = j * n;
			for (int i=0;i<n;i++){
				indices[p++] = (short)(off+i);
				indices[p++] = (short)(off+(i+1)%n);
				indices[p++] = (short)(off+(i+1)%n + n);
				indices[p++] = (short)(off+i + n);
			}
		}
		
		//グーローシェーディング用に法線ベクトルを平均化
		for (int i=0;i<indices.length;i+=4){
			int i1 = indices[i  ];
			int i2 = indices[i+1];
			int i3 = indices[i+2];
			int i4 = indices[i+3];
			//法線ベクトルを求める
			Vector vn = CalcNormal(vertices[i1].v,vertices[i2].v,vertices[i3].v);
			
			//頂点に法線ベクトルを加算
			vertices[i1].n.Add(vn);
			vertices[i2].n.Add(vn);
			vertices[i3].n.Add(vn);
			vertices[i4].n.Add(vn);
			vertices[i1].used ++;
			vertices[i2].used ++;
			vertices[i3].used ++;
			vertices[i4].used ++;
		}
		
		for (int i=0;i<vertices.length;i++){
			//平均化
			if (vertices[i].used > 0){
				vertices[i].n.x /= vertices[i].used;
				vertices[i].n.y /= vertices[i].used;
				vertices[i].n.z /= vertices[i].used;
			}
		}
	}
		
	//頂点バッファを返す
	public VERTEX[] GetVB()
	{
		return vertices;
	}
	
	//インデックス配列を返す
	public short[] GetIndices()
	{
		return indices;
	}
	
	//--------------------------------------------------------------------
	//面に垂直な法線ベクトルを求める。
	//
	//--------------------------------------------------------------------
	static final private Vector CalcNormal(Vector v1,Vector v2,Vector v3)
	{
		Vector n = new Vector();
		Vector a = new Vector();
		Vector b = new Vector();
		
		//３頂点からa,bベクトルを求める。
		a.x = v1.x - v2.x;
		a.y = v1.y - v2.y;
		a.z = v1.z - v2.z;
		b.x = v1.x - v3.x;
		b.y = v1.y - v3.y;
		b.z = v1.z - v3.z;
				
		//外積を取る。
		n.CrossProduct(a,b);
		
		//法線ベクトルを正規化
		n.Normalize();
		
		return n;
	}
}
