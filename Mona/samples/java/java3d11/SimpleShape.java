
//------------------------------------------------------
//シンプルな図形を作成する。
//
//------------------------------------------------------
public class SimpleShape
{
	Vertex vertices[];
	int    indices [];
	
	//箱の作成
	public void CreateBox()
	{
		vertices    = new Vertex[8];
		vertices[0] = new Vertex( 1, 1, 1,0,0,0);
		vertices[1] = new Vertex(-1, 1, 1,0,0,0);
		vertices[2] = new Vertex(-1,-1, 1,0,0,0);
		vertices[3] = new Vertex( 1,-1, 1,0,0,0);
		
		vertices[4] = new Vertex( 1, 1,-1,0,0,0);
		vertices[5] = new Vertex(-1, 1,-1,0,0,0);
		vertices[6] = new Vertex(-1,-1,-1,0,0,0);
		vertices[7] = new Vertex( 1,-1,-1,0,0,0);
		
		int index[] = {
			 0, 1, 2,
			 2, 3, 0,
			 6, 5, 4,
			 4, 7, 6,
			 4, 1, 0,
			 4, 5, 1,
			 6, 3, 2,
			 6, 7, 3,
			 0, 3, 4,
			 3, 7, 4,
			 5, 2, 1,
			 5, 6, 2
		};
		
		//インデックスデータの作成
		indices = new int[12 * 3];
		for (int i=0;i<indices.length;i++){
			indices[i] = index[i];
		}
		
		//グーローシェーディング用に法線ベクトルを平均化
		for (int i=0;i<indices.length;i+=3){
			int i1 = indices[i  ];
			int i2 = indices[i+1];
			int i3 = indices[i+2];
			//法線ベクトルを求める
			Vector vn = CalcNormal(vertices[i1].v,vertices[i2].v,vertices[i3].v);
			
			//頂点に法線ベクトルを加算
			vertices[i1].n.Add(vn);
			vertices[i2].n.Add(vn);
			vertices[i3].n.Add(vn);
			vertices[i1].used ++;
			vertices[i2].used ++;
			vertices[i3].used ++;
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

	//球
	public void CreateSphere(int n)
	{
		float a    = 0;
		float c    = .01f;
		float addc = (float)Math.PI/n - .001f;
		int   p    = 0;
		
		vertices = new Vertex[n * n + n];
		
		for (int i=0;i<=n;i++){
			float s = (float)Math.sin(c);
			float z = (float)Math.cos(c);
			for (int j=0;j<n;j++){
				float x = (float)(Math.cos(a));
				float y = (float)(Math.sin(a));
				vertices[p] = new Vertex();
				vertices[p].v.x = x * s;
				vertices[p].v.y = y * s;
				vertices[p].v.z = z    ;
				a+= Math.PI/n*2;
				p++;
			}
			c += addc;
		}
		
		//インデックスデータの作成
		indices = new int[n * n * 2 * 3];
		
		p = 0;
		int m = n*n + n;
		for (int j=0;j<n;j++){
			int off = j * n;
			for (int i=0;i<n;i++){
				indices[p++] = (off+n)%m+(i+1)%n;
				indices[p++] = off+(i+1)%n;
				indices[p++] = off+i;
				indices[p++] = (off+n)%m+i;
				indices[p++] = (off+n)%m+(i+1)%n;
				indices[p++] = off+i;
			}
		}
		
		//グーローシェーディング用に法線ベクトルを平均化
		for (int i=0;i<indices.length;i+=3){
			int i1 = indices[i  ];
			int i2 = indices[i+1];
			int i3 = indices[i+2];
			//法線ベクトルを求める
			Vector vn = CalcNormal(vertices[i1].v,vertices[i2].v,vertices[i3].v);
			
			//頂点に法線ベクトルを加算
			vertices[i1].n.Add(vn);
			vertices[i2].n.Add(vn);
			vertices[i3].n.Add(vn);
			vertices[i1].used ++;
			vertices[i2].used ++;
			vertices[i3].used ++;
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
	public Vertex[] GetVB()
	{
		return vertices;
	}
	
	//インデックス配列を返す
	public int[] GetIndices()
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
