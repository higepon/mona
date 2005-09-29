
//------------------------------------------------------
//シンプルな図形を作成する。
//
//
//------------------------------------------------------
public final class SimpleShape
{	
	//球
	public static PRIMITIVE createSphere(int n)
	{
		//頂点データの作成
		float a = 0,c = .01f,addc = (float)Math.PI/n - .001f;
		int   p = 0;

		VERTEX[] vertices = new VERTEX[n * n + n];
		
		for (int i=0;i<=n;i++){			
			float s = (float)Math.sin(c);
			float z = (float)Math.cos(c);
			for (int j=0;j<n;j++){				
				float x = (float)(Math.cos(a));
				float y = (float)(Math.sin(a));
				vertices[p] = new VERTEX();
				vertices[p].x = x * s;
				vertices[p].y = y * s;
				vertices[p].z = z    ;
				vertices[p].work = 0;
				a+= Math.PI/n*2;
				p++;
			}
			c += addc;
		}
		
		//インデックスデータの作成
		short[] indices = new short[n * n * 2 * 3];
		
		p = 0;
		int m = n*n + n;
		for (int j=0;j<n;j++){
			int off = j * n;
			for (int i=0;i<n;i++){
				indices[p++] = (short)((off+n)%m+(i+1)%n);
				indices[p++] = (short)((off  )  +(i+1)%n);
				indices[p++] = (short)((off+i)          );
				indices[p++] = (short)((off+n)%m+(i  )  );
				indices[p++] = (short)((off+n)%m+(i+1)%n);
				indices[p++] = (short)((off+i)          );
			}
		}
		
		//プリミティブ設定
		PRIMITIVE pm = new PRIMITIVE();
		pm.vertex = vertices;
		pm.index  = indices;
		pm.color  = 0xffffffff;
		pm.diff   = 1;
		pm.glos   = 4;
		pm.spec   = 1;
		pm.texture= null;

		//法線ベクトル設定
		setNormal(pm);
		
		return pm;
	}
	
	//ドーナツ
	public static PRIMITIVE createDonuts(int n)
	{
		float a = 0,c = 0,addc = (float)Math.PI*2/n;
		int   p = 0;
		
		//頂点データの作成
		VERTEX[] vertices = new VERTEX[n * n];
		
		for (int i=0;i<n;i++){
			float s = (float)Math.sin(c) + 2;
			float z = (float)Math.cos(c);
			for (int j=0;j<n;j++){
				float x = (float)(Math.cos(a));
				float y = (float)(Math.sin(a));
				vertices[p] = new VERTEX();
				vertices[p].x = x * s * .4f;
				vertices[p].y = y * s * .4f;
				vertices[p].z = z * .4f;
				vertices[p].work = 0;
				a+= Math.PI/n*2;
				p++;
			}
			c += addc;
		}
		
		//インデックスデータの作成
		short[] indices = new short[n * n * 2 * 3];
		
		p = 0;
		int m = n*n;
		for (int j=0;j<n;j++){
			int off = j * n;
			for (int i=0;i<n;i++){
				indices[p++] = (short)((off+n)%m+(i+1)%n);
				indices[p++] = (short)((off  )  +(i+1)%n);
				indices[p++] = (short)((off  )  +(i  )  );

				indices[p++] = (short)((off+n)%m+(i  )  );
				indices[p++] = (short)((off+n)%m+(i+1)%n);
				indices[p++] = (short)((off  )  +(i  )  );
			}
		}
		
		//プリミティブ設定
		PRIMITIVE pm = new PRIMITIVE();
		pm.vertex = vertices;
		pm.index  = indices;
		pm.color  = 0xffffffff;
		pm.diff   = 1;
		pm.glos   = 4;
		pm.spec   = 1;
		pm.texture= null;

		//法線ベクトル設定
		setNormal(pm);
		
		return pm;
	}

	//グーローシェーディング用に法線ベクトルを平均化
	public static void setNormal(PRIMITIVE pm)
	{
		VERTEX[] vertices = pm.vertex;
		short[]  indices  = pm.index;
		
		for (int i=0;i<indices.length;i+=3){
			
			VERTEX v1 = vertices[indices[i  ]];
			VERTEX v2 = vertices[indices[i+1]];
			VERTEX v3 = vertices[indices[i+2]];

			float ax = v2.x - v1.x;
			float ay = v2.y - v1.y;
			float az = v2.z - v1.z;
			float bx = v3.x - v2.x;
			float by = v3.y - v2.y;
			float bz = v3.z - v2.z;
			
			//外積
			float nx = ay * bz - az * by;
			float ny = az * bx - ax * bz;
			float nz = ax * by - ay * bx;
			
			//正規化
			float rhl = (float)(1.0 / Math.sqrt(nx * nx + ny * ny + nz * nz));
			nx *= rhl;
			ny *= rhl;
			nz *= rhl;
						
			//法線ベクトルを加算
			v1.nx += nx; v1.ny += ny; v1.nz += nz; v1.work++;
			v2.nx += nx; v2.ny += ny; v2.nz += nz; v2.work++;
			v3.nx += nx; v3.ny += ny; v3.nz += nz; v3.work++;			
		}

		//平均化		
		for (int i=0;i<vertices.length;i++){
			if (vertices[i].work > 0){
				vertices[i].nx /= vertices[i].work;
				vertices[i].ny /= vertices[i].work;
				vertices[i].nz /= vertices[i].work;
			}
		}
	}
}
