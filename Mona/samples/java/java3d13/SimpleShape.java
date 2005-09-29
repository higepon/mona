
//------------------------------------------------------
//シンプルな図形を作成する。
//
//トーラス
//------------------------------------------------------
public class SimpleShape
{
	Vertex vertices[];
	int    indices [];
	
	//ドーナツ
	public void CreateDonuts(int n)
	{
		float a    = 0;
		float c    = 0;
		float addc = (float)Math.PI*2/n;
		int   p    = 0;
		
		//頂点データの作成
		vertices = new Vertex[n * n];
		
		for (int i=0;i<n;i++){
			float s = (float)Math.sin(c) + 2;
			float z = (float)Math.cos(c);
			for (int j=0;j<n;j++){
				float x = (float)(Math.cos(a));
				float y = (float)(Math.sin(a)) * .8f;
				vertices[p] = new Vertex();
				vertices[p].v.x = x * s * .4f;
				vertices[p].v.y = y * s * .4f;
				vertices[p].v.z = z * .4f;
				a+= Math.PI/n*2;
				p++;
			}
			c += addc;
		}
		
		//インデックスデータの作成
		indices = new int[n * n * 2 * 3];
		
		p = 0;
		int m = n*n;
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
			//UV座標設定
			vertices[i].tu = vertices[i].v.x/2+.5f;
			vertices[i].tv = vertices[i].v.y/2+.5f;
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
		n.Invert();
		
		return n;
	}
	
	//--------------------------------------------------------------------
	//バウンディングボックスを作成する
	//
	//--------------------------------------------------------------------
	final public void CreateBoundingBox(Vertex[] vtx)
	{
		float minx,miny,minz;
		float maxx,maxy,maxz;
		minx = miny = minz = +0x7fffff;
		maxx = maxy = maxz = -0x7fffff;
		
		for (int i=0;i<vtx.length;i++){
			Vector v = vtx[i].v;
			if (v.x < minx) minx = v.x; 
			if (v.y < miny) miny = v.y;
			if (v.z < minz) minz = v.z;
			if (v.x > maxx) maxx = v.x; 
			if (v.y > maxy) maxy = v.y;
			if (v.z > maxz) maxz = v.z;
		}
		
		vertices = new Vertex[8];
		vertices[0] = new Vertex(minx,miny,minz);
		vertices[1] = new Vertex(minx,miny,maxz);
		vertices[2] = new Vertex(minx,maxy,minz);
		vertices[3] = new Vertex(minx,maxy,maxz);
		vertices[4] = new Vertex(maxx,miny,minz);
		vertices[5] = new Vertex(maxx,miny,maxz);
		vertices[6] = new Vertex(maxx,maxy,minz);
		vertices[7] = new Vertex(maxx,maxy,maxz);
		
		indices = new int[3 * 12];
		
		indices[ 0] = 0;
		indices[ 1] = 1;
		indices[ 2] = 2;
		indices[ 3] = 1;
		indices[ 4] = 2;
		indices[ 5] = 3;
		
		indices[ 6] = 4;
		indices[ 7] = 5;
		indices[ 8] = 6;
		indices[ 9] = 5;
		indices[10] = 6;
		indices[11] = 7;
		
		indices[12] = 0;
		indices[13] = 1;
		indices[14] = 5;
		indices[15] = 0;
		indices[16] = 5;
		indices[17] = 4;
		
		indices[18] = 2;
		indices[19] = 3;
		indices[20] = 7;
		indices[21] = 2;
		indices[22] = 7;
		indices[23] = 6;
		
		indices[24] = 1;
		indices[25] = 3;
		indices[26] = 7;
		indices[27] = 1;
		indices[28] = 7;
		indices[29] = 5;
		
		indices[30] = 0;
		indices[31] = 2;
		indices[32] = 6;
		indices[33] = 0;
		indices[34] = 6;
		indices[35] = 4;
	}
	
	//--------------------------------------------------------------------
	//バウンディングスフィアを作成する
	//
	//--------------------------------------------------------------------
	final public void CreateBoundingSphere(Vertex[] vtx,int n)
	{
		float minx,miny,minz;
		float maxx,maxy,maxz;
		minx = miny = minz = +0x7fffff;
		maxx = maxy = maxz = -0x7fffff;
		
		for (int i=0;i<vtx.length;i++){
			Vector v = vtx[i].v;
			if (v.x < minx) minx = v.x; 
			if (v.y < miny) miny = v.y;
			if (v.z < minz) minz = v.z;
			if (v.x > maxx) maxx = v.x; 
			if (v.y > maxy) maxy = v.y;
			if (v.z > maxz) maxz = v.z;
		}
		
		//重心を求める
		Vector c = new Vector((minx + maxx)/2,(miny + maxy)/2,(minz + maxz)/2);
		
		//重心からの最長距離を求める
		float r = 0;
		for (int i=0;i<vtx.length;i++){
			//重心からの距離を求める
			float dx = vtx[i].v.x - c.x;
			float dy = vtx[i].v.y - c.y;
			float dz = vtx[i].v.z - c.z;
			float dr = (float)Math.sqrt(dx * dx + dy * dy + dz * dz);
			//比較
			if (dr > r) r = dr;
		}
		
		//バウンディングスフィアは中心がc,半径がrの球である。
		
		//視覚的確認のために球のモデルを作成
		float a    = 0;
		float w    = .01f;
		float addw = (float)Math.PI/n - .001f;
		int   p    = 0;
		
		vertices = new Vertex[n * n + n];
		
		for (int i=0;i<=n;i++){
			float s = (float)Math.sin(w);
			float z = (float)Math.cos(w) * r;
			for (int j=0;j<n;j++){
				float x = (float)(Math.cos(a)) * r;
				float y = (float)(Math.sin(a)) * r;
				vertices[p] = new Vertex();
				vertices[p].v.x = x * s + c.x;
				vertices[p].v.y = y * s + c.y;
				vertices[p].v.z = z     + c.z;
				a+= Math.PI/n*2;
				p++;
			}
			w += addw;
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
	}
}
