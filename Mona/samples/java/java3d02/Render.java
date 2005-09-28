/*
import java.awt.*;
import java.applet.*;
import java.awt.image.*;
*/

//------------------------------------------------------
//レンダー
//
//------------------------------------------------------
public class Render
{	
	static final int PRIMITIVE_POLYGON = 1;
	
	static Matrix mMaster; // = new Matrix();
								
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ int pbuf[];		//32bitピクセルバッファ
	static private int zbuf[];		//32bitＺバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
	static private int CENTERX;
	static private int CENTERY;
	
	static private int min [];
	static private int max [];
	static private int minz[];
	static /*private*/ int maxz[];
	
	//--------------------------------------------------------------------
	//コンストラクタ
	//
	//--------------------------------------------------------------------
	public Render(int w,int h)
	{
		mMaster = new Matrix();
		
		BUFFERW = w;
		BUFFERH = h;
		CENTERX = BUFFERW/2;
		CENTERY = BUFFERH/2;
		
		pbuf = new int[BUFFERW * BUFFERH];
		SimpleCanvas.create(pbuf, "FlatShading", BUFFERW, BUFFERH);
		zbuf = new int[BUFFERW * BUFFERH];
		min  = new int[BUFFERH];
		max  = new int[BUFFERH];
		minz = new int[BUFFERH];
		maxz = new int[BUFFERH];
	}

	//--------------------------------------------------------------------
	//バッファのクリア
	//
	//--------------------------------------------------------------------
	static final public void Clear()
	{
		for (int i=0;i<BUFFERW * BUFFERH;i++){
				pbuf[i] = 0xffffffff;
				zbuf[i] = 2147483647;
		}
	}
	
	/*
	//--------------------------------------------------------------------
	//レンダリング開始
	//
	//--------------------------------------------------------------------
	static final public void BeginScene()
	{
	}
	
	//--------------------------------------------------------------------
	//レンダリング終了
	//
	//Applet applet ...アプレットへの参照
	//Graphics back ...オフスクリーンへの参照
	//--------------------------------------------------------------------
	static final public void EndScene(Applet applet,Graphics offscreen)
	{
		offscreen.drawImage(applet.createImage(new MemoryImageSource(BUFFERW,BUFFERH,pbuf,0,BUFFERW)),0,0,null);
	}
	*/

	//--------------------------------------------------------------------
	//変換行列を設定する。
	//
	//Matrix m ...変換行列
	//--------------------------------------------------------------------
	static final public void SetTransform(Matrix m)
	{
		mMaster.Initialize(m);
	}

	//--------------------------------------------------------------------
	//プリミティブ描画
	//
	//--------------------------------------------------------------------
	static final public void DrawIndexedPrimitive(int type,Vector[] vertices,int[] indices)
	{
		Vector[] tvtx = new Vector[vertices.length];
		
		//座標変換
		for (int i=0;i<tvtx.length;i++)
			tvtx[i] = mMaster.Transform(vertices[i]);
	
		//ラスタライズ
		switch(type){
		case PRIMITIVE_POLYGON:
			for (int i=0;i<indices.length;i+=3){
				DrawPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		}
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
	
	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(Vector v1,Vector v2,Vector v3)
	{
		//視線ベクトル兼、平行光源ベクトル
		Vector eye = new Vector(0.0f,0.0f,-1.0f);
		
		//面の法線ベクトルを求める。
		Vector n = CalcNormal(v1,v2,v3);	
		
		//光源と法線の内積を取る。
		float w = n.DotProduct(eye);
	
		//内積が負なら裏面なので描画しない。（ただしパースをつけた場合は正確な判定方法ではない。）
		if (w < 0) return;
		
		//面の色を決める。
		Vector a = new Vector(.2f,.2f,.2f);	//アンビエント（環境光）の色
		Vector d = new Vector(.8f,.8f,.8f);	//ディフューズ（拡散光）の色

		int r = (int)((a.x + w * d.x) * 255);
		int g = (int)((a.y + w * d.y) * 255);
		int b = (int)((a.z + w * d.z) * 255);
		
		if (r < 0) r = 0; else if (r > 255) r = 255;
		if (g < 0) g = 0; else if (g > 255) g = 255;
		if (b < 0) b = 0; else if (b > 255) b = 255;
		
		//面の色
		int color = (r<<16) | (g<<8) | b | 0xff000000;
 		
		//バッファの使用範囲を設定（高速化のため）
		int top = +2147483647;
		int btm = -2147483648;
		if (top > (int)v1.y ) top = (int)v1.y;
		if (top > (int)v2.y ) top = (int)v2.y;
		if (top > (int)v3.y ) top = (int)v3.y;
		if (btm < (int)v1.y ) btm = (int)v1.y;
		if (btm < (int)v2.y ) btm = (int)v2.y;
		if (btm < (int)v3.y ) btm = (int)v3.y;
		if (top < 0         ) top = 0;
		if (btm > BUFFERH   ) btm = BUFFERH;
				
		//最大最小バッファの初期化
		for (int i=top;i<btm;i++){
			min[i] = +2147483647;
			max[i] = -2147483648;
		}
		
		//スキャンエッジ
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v1);
		
		//最大最小バッファに基づいて描画する。
		for (int y=top;y<btm;y++){
			
			//バッファが未更新ならスキップ
			if (min[y] == +2147483647) continue;
			
			final int offset = y*BUFFERW;
			
			//増分値計算
			int l = (max[y] - min[y]) + 1;
			int addz = (maxz[y] - minz[y]) / l;

			//初期値設定
			int z = minz[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz){
			
				if (x < 0 || x >=BUFFERW) continue;
				
				int p = offset + x;

				if (zbuf[p] > z){
					pbuf[p] = color;
					zbuf[p] = z;
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//スキャンライン
	//
	//Vector v1 ...始点
	//Vector v2 ...終点
	//--------------------------------------------------------------------
	static final private void ScanEdge(Vector v1,Vector v2)
	{		
		int l = Math.abs((int)(v2.y - v1.y)) + 1;
	
		//増分値計算
		int addx = (int)((v2.x - v1.x) * 0xffff) / l;
		int addy = (int)((v2.y - v1.y) * 0xffff) / l;
		int addz = (int)((v2.z - v1.z) * 0xffff) / l;
		
		//初期値設定
		int x = (int)(v1.x * 0xffff);
		int y = (int)(v1.y * 0xffff);
		int z = (int)(v1.z * 0xffff);
		
		//スキャン
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < 0 || py >= BUFFERH) continue;
			
			if (min [py] > px){
				min [py] = px;
				minz[py] = z;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
			}
		}
	}
}
