/*
import java.awt.*;
import java.applet.*;
import java.awt.image.*;
*/

//-----------------------------------------------------------
//照明・変換済みの頂点
//
//-----------------------------------------------------------
final class TLVERTEX
{
	int x;//頂点座標
	int y;
	int z;
	int r;
	int g;
	int b;
	int u;
	int v;
}

//-----------------------------------------------------------
//頂点
//
//-----------------------------------------------------------
final class VERTEX
{
	Vector v = new Vector();//頂点座標
	Vector n = new Vector();//法線ベクトル
	float tu;	//テクスチャ座標
	float tv;	
	int used;	//使用回数
}

//------------------------------------------------------
//レンダー
//
//------------------------------------------------------
public final class Render
{	
	static final int PRIMITIVE_POLYLINE   = 2;
	static final int PRIMITIVE_POLYAALINE = 3;
	
	static Matrix mMaster; // = new Matrix();
								
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ public int pbuf[];		//32bitピクセルバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
		
	static private TLVERTEX tvtx[];
	
	//--------------------------------------------------------------------
	//コンストラクタ
	//
	//--------------------------------------------------------------------
	public Render(int w,int h)
	{
		mMaster = new Matrix();
		
		BUFFERW = w;
		BUFFERH = h;
		
		pbuf = new int[BUFFERW * BUFFERH];
				
		tvtx = new TLVERTEX[2048];
				
		for (int i=0;i<2048;i++)
			tvtx[i] = new TLVERTEX();
	}

	//--------------------------------------------------------------------
	//バッファのクリア
	//
	//--------------------------------------------------------------------
	static final public void Clear()
	{
		for (int i=0;i<BUFFERW * BUFFERH;i++){
				pbuf[i] = 0xffffffff;
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
	static final public void DrawIndexedPrimitive(int type,VERTEX[] vertices,short[] indices)
	{
		//座標変換
		Transform(vertices);
		
		//ラスタライズ
		switch(type){
		case PRIMITIVE_POLYLINE:
			for (int i=0;i<indices.length;i+=4){
				DrawPolyLine(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]],tvtx[indices[i+3]]);
			}
			break;
		case PRIMITIVE_POLYAALINE:
			for (int i=0;i<indices.length;i+=4){
				DrawPolyAALine(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]],tvtx[indices[i+3]]);
			}
			break;
		}
	}

	//--------------------------------------------------------------------
	//座標変換
	//
	//--------------------------------------------------------------------
	static final public void Transform(VERTEX[] vtx)
	{		
		Matrix m = mMaster;
		
		for (int i=0;i<vtx.length;i++){						
			//座標変換
			Vector v = vtx[i].v;
			float x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30;
			float y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31;
			float z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32;
	
			//頂点情報の設定
			tvtx[i].x = (int)(x * 0xffff);
			tvtx[i].y = (int)(y * 0xffff);
			tvtx[i].z = (int)(z * 0xffff);
		}
	}

	//--------------------------------------------------------------------
	//ポリゴンライン
	//
	//--------------------------------------------------------------------
	static final private void DrawPolyLine(TLVERTEX v1,TLVERTEX v2,TLVERTEX v3,TLVERTEX v4)
	{
		//カリング（裏面消去）
//		if (((v2.x-v1.x)>>8) * ((v3.y-v1.y)>>8) - ((v2.y-v1.y)>>8) * ((v3.x-v1.x)>>8) <= 0)
//			return;
		
		int color = 0xff4040ff;

		//アンチエイリアスライン
		DrawLine(v1,v2,color);
		DrawLine(v2,v3,color);
//		DrawLine(v3,v4,color);
//		DrawLine(v4,v1,color);
	}

	//--------------------------------------------------------------------
	//ポリゴンライン（アンチエイリアス）
	//
	//--------------------------------------------------------------------
	static final private void DrawPolyAALine(TLVERTEX v1,TLVERTEX v2,TLVERTEX v3,TLVERTEX v4)
	{		
		int color = 0xff4040ff;
		
		//アンチエイリアスライン
		DrawAALine(v1,v2,color);
		DrawAALine(v2,v3,color);
//		DrawAALine(v3,v4,color);
//		DrawAALine(v4,v1,color);
	}

	//--------------------------------------------------------------------
	//ライン
	//
	//TLVERTEX v1 ...始点
	//TLVERTEX v2 ...終点
	//--------------------------------------------------------------------
	static final private void DrawLine(TLVERTEX v1,TLVERTEX v2,int color)
	{
		int lx = Math.abs((v2.x>>16) - (v1.x>>16));
		int ly = Math.abs((v2.y>>16) - (v1.y>>16));

		if (lx < ly){			
			if (ly == 0) ly++;
			int ax = (v2.x - v1.x) / ly;
			int ay = ((v2.y-v1.y)>>31) | 1;
			int x  = v1.x;
			int y  = v1.y>>16;
		
			for (int i=0;i<ly;i++,x+=ax,y+=ay){
				int px = x>>16;
				pbuf[y * BUFFERW + px] = color;
			}
		}else{
			if (lx == 0) lx++;
			int ay = (v2.y - v1.y) / lx;
			int ax = ((v2.x-v1.x)>>31) | 1;
			int x  = v1.x>>16;
			int y  = v1.y;
		
			for (int i=0;i<lx;i++,x+=ax,y+=ay){
				int py = y>>16;
				pbuf[py * BUFFERW + x] = color;
			}
		}
	}

	//--------------------------------------------------------------------
	//アンチエイリアスライン
	//
	//TLVERTEX v1 ...始点
	//TLVERTEX v2 ...終点
	//--------------------------------------------------------------------
	static final private void DrawAALine(TLVERTEX v1,TLVERTEX v2,int color)
	{
		//クリッピング処理はしていません。
		
		int lx = Math.abs((v2.x>>16) - (v1.x>>16));
		int ly = Math.abs((v2.y>>16) - (v1.y>>16));

		if (lx < ly){			
			if (ly == 0) ly++;
			
			int ax = (v2.x - v1.x) / ly;
			int ay = ((v2.y-v1.y)>>31) | 1;
			int x  = v1.x;
			int y  = v1.y>>16;
		
			for (int i=0;i<ly;i++,x+=ax,y+=ay){
				int px = x>>16;
				
				int p1 = y * BUFFERW + px;
				int p2 = p1 + 1;			
				int rx = x & 0xffff;
				pbuf[p1] = AlphaBlend(pbuf[p1],color,       rx);
				pbuf[p2] = AlphaBlend(pbuf[p2],color,0xffff-rx);
			}
		}else{
			if (lx == 0) lx++;
			
			int ay = (v2.y - v1.y) / lx;
			int ax = ((v2.x-v1.x)>>31) | 1;
			int x  = v1.x>>16;
			int y  = v1.y;
		
			for (int i=0;i<lx;i++,x+=ax,y+=ay){
				int py = y>>16;
				
				int p1 = py * BUFFERW + x;
				int p2 = p1 + BUFFERW;
				int ry = y & 0xffff;
				pbuf[p1] = AlphaBlend(pbuf[p1],color,       ry);
				pbuf[p2] = AlphaBlend(pbuf[p2],color,0xffff-ry);
			}
		}
	}
		
	static final private int AlphaBlend(int a,int b,int rate)
	{		
		int tmp;
		
		tmp = (b & 0xff0000)>>16;
		int R = ((((a & 0xff0000)>>16) - tmp) * rate >>16) + tmp;
		tmp = (b & 0x00ff00)>>8;
		int G = ((((a & 0x00ff00)>>8 ) - tmp) * rate >>16) + tmp;
		tmp = (b & 0x0000ff);
		int B = ((((a & 0x0000ff)    ) - tmp) * rate >>16) + tmp;

		
		return (R<<16) | (G<<8) | B | 0xff000000;
	}
}
