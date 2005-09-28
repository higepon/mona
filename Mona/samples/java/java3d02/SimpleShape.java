
//------------------------------------------------------
//シンプルな図形を作成する。
//
//トーラス
//------------------------------------------------------
public class SimpleShape
{
	Vector vertices[];
	int    indices [];
	
	//ドーナツ
	public void CreateDonuts(int n)
	{
		float a    = 0;
		float c    = 0;
		float addc = (float)Math.PI*2/n;
		int   p    = 0;
		
		vertices = new Vector[n * n];
		
		for (int i=0;i<n;i++){
			float s = (float)Math.sin(c) + 2;
			float z = (float)Math.cos(c);
			for (int j=0;j<n;j++){
				float x = (float)(Math.cos(a));
				float y = (float)(Math.sin(a));
				vertices[p] = new Vector();
				vertices[p].x = x * s * .4f;
				vertices[p].y = y * s * .4f;
				vertices[p].z = z * .4f;
			
				a+= Math.PI/n*2;
				p++;
			}
			c += addc;
		}
		
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
	}
		
	//頂点バッファを返す
	public Vector[] GetVB()
	{
		return vertices;
	}
	
	//インデックス配列を返す
	public int[] GetIndices()
	{
		return indices;
	}
}
