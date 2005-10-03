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
	int r;//頂点色
	int g;
	int b;
	int u;//テクスチャ座標
	int v;
	int s;//スペキュラー
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

//-----------------------------------------------------------
//スキャンラインバッファ
//
//-----------------------------------------------------------
final class SCANLINE
{
	int x;
	int z;
	int r;
	int g;
	int b;
	int u;
	int v;
	int s;
}

//------------------------------------------------------
//レンダー
//
//------------------------------------------------------
public final class Render
{	
	static final int PRIMITIVE_POLYGON = 1;
	
	static Matrix mMaster; // = new Matrix();
								
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ int pbuf[];		//32bitピクセルバッファ
	static private int zbuf[];		//32bitＺバッファ
	static private int tbuf[];		//32bitテクスチャバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
	static private int TEXTUREW;
	static private int TEXTUREH;
	static private int TEXMASKW;
	static private int TEXMASKH;
	static private int TEXSHIFT;
	
	static private SCANLINE min[];
	static private SCANLINE max[];
	
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
		zbuf = new int[BUFFERW * BUFFERH];
		
		min  = new SCANLINE[BUFFERH];
		max  = new SCANLINE[BUFFERH];
		
		tvtx = new TLVERTEX[2048];
		
		for (int i=0;i<BUFFERH;i++){
			min[i] = new SCANLINE();
			max[i] = new SCANLINE();
		}
		
		for (int i=0;i<2048;i++)
			tvtx[i] = new TLVERTEX();
	}

	//--------------------------------------------------------------------
	//テクスチャの設定
	//
	//--------------------------------------------------------------------
	static final public void SetTexture(/*Image*/MyTexture texture)
	{
		TEXTUREW  = texture.width;  //.getWidth (null);
		TEXTUREH  = texture.height; //.getHeight(null);
		TEXMASKW  = TEXTUREW - 1;
		TEXMASKH  = TEXTUREH - 1;
		
		TEXSHIFT = 0;
		for (int i=0;i<32;i++)
			if (((TEXMASKW>>i)&1) > 0)
				TEXSHIFT++;
		
		/*
		tbuf = new int[TEXTUREW * TEXTUREH];
		
		PixelGrabber pg = new PixelGrabber(texture,0,0,TEXTUREW,TEXTUREH,tbuf,0,TEXTUREW);
		
		try{
			pg.grabPixels();
		}catch(InterruptedException e){}
		*/
		tbuf = texture.data;
	}

	//--------------------------------------------------------------------
	//バッファのクリア
	//
	//--------------------------------------------------------------------
	static final public void Clear()
	{
		for (int i=0;i<BUFFERW * BUFFERH;i++){
				pbuf[i] = 0xffffffff;
				zbuf[i] = 0x7fffffff;
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
		//座標変換、ライティング
		TransformAndLighting(vertices);
		
		//ラスタライズ
		switch(type){
		case PRIMITIVE_POLYGON:
			for (int i=0;i<indices.length;i+=4){
				DrawPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]],tvtx[indices[i+3]]);
			}
			break;
		}
	}

	//--------------------------------------------------------------------
	//座標変換、ライティング
	//
	//--------------------------------------------------------------------
	static final public void TransformAndLighting(VERTEX[] vtx)
	{		
		//転置行列の作成
		Matrix  m = mMaster;
		Matrix im = new Matrix();
		im.Invert(mMaster);
		
		//平行光源の向き
		Vector light = im.Transform(new Vector(1,2,1.5f));
		light.Normalize();
		
		Vector eye   = im.Transform( new Vector(0,0,1));
		eye.Normalize();
		
		Vector n  = new Vector();
		
		for (int i=0;i<vtx.length;i++){
						
			//座標変換
			Vector v = vtx[i].v;
			float x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30;
			float y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31;
			float z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32;
	
			//頂点法線と光源とのなす角
			float w = light.DotProduct(vtx[i].n);
			if (w < 0) w = 0;
	
			//頂点色を求める
			int r = (int)(w * 255 + 32);
			int g = (int)(w * 255 + 32);
			int b = (int)(w * 255 + 32);
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			
			//スペキュラーを求める
			n.Initialize(vtx[i].n);
			n.x = n.x * 2 * w - light.x;
			n.y = n.y * 2 * w - light.y;
			n.z = n.z * 2 * w - light.z;
			n.Normalize();
						
			float sw = n.DotProduct(eye);//視線ベクトルと反射ベクトルとの内積
			if (sw < 0) sw = 0;
			sw = (float)Math.pow(sw,8);//収束			
			int s = (int)(sw * 255);
							
			//頂点情報の設定
			tvtx[i].x = (int)(x * 0xffff);
			tvtx[i].y = (int)(y * 0xffff);
			tvtx[i].z = (int)(z * 0xffff);
			tvtx[i].r = r<<16;
			tvtx[i].g = g<<16;
			tvtx[i].b = b<<16;
			tvtx[i].s = s<<16;
		
			
			//テクスチャ座標設定
			tvtx[i].u = (int)(vtx[i].tu * TEXTUREW * 0xffff);
			tvtx[i].v = (int)(vtx[i].tv * TEXTUREH * 0xffff);
		}
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(TLVERTEX v1,TLVERTEX v2,TLVERTEX v3,TLVERTEX v4)
	{
		//カリング（裏面消去）
		if (((v2.x-v1.x)>>8) * ((v3.y-v1.y)>>8) - ((v2.y-v1.y)>>8) * ((v3.x-v1.x)>>8) <= 0)
			return;
		
		//バッファの使用範囲を設定（高速化のため）
		int top = +2147483647;
		int btm = -2147483648;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if (top > v4.y ) top = v4.y;
		if (btm < v1.y ) btm = v1.y;
		if (btm < v2.y ) btm = v2.y;
		if (btm < v3.y ) btm = v3.y;
		if (btm < v4.y ) btm = v4.y;
		top>>=16;
		btm>>=16;
		if (top < 0         ) top = 0;
		if (btm > BUFFERH   ) btm = BUFFERH;
				
		//最大最小バッファの初期化
		for (int i=top;i<btm;i++){
			min[i].x = +2147483647;
			max[i].x = -2147483648;
		}
		
		//エッジをスキャン
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v4);
		ScanEdge(v4,v1);
		
		for (int y=top;y<btm;y++){
			
			//バッファが未更新ならスキップ
			if (min[y].x == +2147483647) continue;
			
			final int offset = y*BUFFERW;
			
			SCANLINE pmin = min[y];
			SCANLINE pmax = max[y];
			
			//増分値計算
			int l  = (pmax.x - pmin.x) + 1;
			int az = (pmax.z - pmin.z) / l;
			int ar = (pmax.r - pmin.r) / l;
			int ag = (pmax.g - pmin.g) / l;
			int ab = (pmax.b - pmin.b) / l;
			int au = (pmax.u - pmin.u) / l;
			int av = (pmax.v - pmin.v) / l;
			int as = (pmax.s - pmin.s) / l;

			//初期値設定
			int z  = pmin.z;
			int r  = pmin.r;
			int g  = pmin.g;
			int b  = pmin.b;
			int u  = pmin.u;
			int v  = pmin.v;
			int s  = pmin.s;
			
			for (int x=pmin.x;x<=pmax.x;x++,z+=az,r+=ar,g+=ag,b+=ab,u+=au,v+=av,s+=as){
			
				if (x < 0 || x >=BUFFERW) continue;
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] > z){
					//テクセルを取得
					int texel = tbuf[(((v>>16)&TEXMASKH)<<TEXSHIFT) + ((u>>16)&TEXMASKW)];
					
					//テクセルのRGBと頂点色から補完したRGBを乗算
					int tr = ( (r>>16) * ((texel & 0xff0000)>>16) )>>8;
					int tg = ( (g>>16) * ((texel & 0x00ff00)>>8 ) )>>8;
					int tb = ( (b>>16) * ((texel & 0x0000ff)    ) )>>8;
					
					int color = (tr<<16) | (tg<<8) | tb;
					int spec  = (s&0xff0000) | ((s>>8)&0xff00) | (s>>16);
												
					//スペキュラーを加算
					int c = (color & 0x00fefefe) + (spec & 0x00fefefe);
					int d = c;
					
					c &= 0x01010100;
					c -= c >> 8;
					
					pbuf[p] = c | d | 0xff000000;
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
	static final private void ScanEdge(TLVERTEX v1,TLVERTEX v2)
	{		
		int l = Math.abs((int)((v2.y>>16) - (v1.y>>16))) + 1;
			
		//増分値計算
		int ax = (v2.x - v1.x) / l;
		int ay = (v2.y - v1.y) / l;
		int az = (v2.z - v1.z) / l;
		int ar = (v2.r - v1.r) / l;
		int ag = (v2.g - v1.g) / l;
		int ab = (v2.b - v1.b) / l;
		int au = (v2.u - v1.u) / l;
		int av = (v2.v - v1.v) / l;
		int as = (v2.s - v1.s) / l;
		
		//初期値設定
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int r = v1.r;
		int g = v1.g;
		int b = v1.b;
		int u = v1.u;
		int v = v1.v;
		int s = v1.s;
		
		//スキャン
		for (int i=0;i<l;i++,x+=ax,y+=ay,z+=az,r+=ar,g+=ag,b+=ab,u+=au,v+=av,s+=as){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < 0 || py >= BUFFERH) continue;			

			SCANLINE pmin = min[py];
			SCANLINE pmax = max[py];
			
			if (pmin.x > px){
				pmin.x = px;
				pmin.z = z;
				pmin.r = r;
				pmin.g = g;
				pmin.b = b;
				pmin.u = u;
				pmin.v = v;
				pmin.s = s;
			}
			
			if (pmax.x < px){
				pmax.x = px;
				pmax.z = z;
				pmax.r = r;
				pmax.g = g;
				pmax.b = b;
				pmax.u = u;
				pmax.v = v;
				pmax.s = s;
			}
		}
	}
}
