/*
import java.awt.*;
import java.applet.*;
import java.awt.image.*;
*/

//-----------------------------------------------------------
//プリミティブ
//
//-----------------------------------------------------------
final class PRIMITIVE{
	VERTEX[]  vertex;
	short[]   index;
	
	int   color;
	int   glos;
	float spec;
	float diff;

	TEXTURE texture;
}

//-----------------------------------------------------------
//テクスチャ
//
//-----------------------------------------------------------
final class TEXTURE{
	String name;
	
	int[] img;
	int   w;
	int   h;
	int   shift;
}

//-----------------------------------------------------------
//頂点
//
//-----------------------------------------------------------
final class VERTEX{
	float x;	//頂点座標
	float y;
	float z;
	float nx;	//法線ベクトル
	float ny;
	float nz;
	float tu;	//テクスチャ座標
	float tv;
	int work;
}

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
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ int pbuf[];		//32bitピクセルバッファ
	static private int zbuf[];		//32bitＺバッファ
	static private int tbuf[];
	static private int abuf[];		//アキュムレーションバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
	
	static private SCANLINE min[];
	static private SCANLINE max[];
	
	static final int MAX_VERTEX  = 4096;	//最大頂点数
	
	static private TLVERTEX tvtx[];
	
	static final int TRANSFORM_WORLD  = 0;
	static final int TRANSFORM_VIEW   = 2;
	static final int TRANSFORM_PROJ   = 3;
	static final int TRANSFORM_MASTER = 4;

	static int TEXTUREW;
	static int TEXTUREH;
	static int TEXMASKW;
	static int TEXMASKH;
	static int TEXSHIFT;
		
	static Matrix mWorld;  // = new Matrix();//ワールド変換行列
	static Matrix mMaster; // = new Matrix();//マスター行列
	static Matrix mView;   // = new Matrix();//ビュー変換行列
	static Matrix mProj;   // = new Matrix();//射影変換行列	
	
	static Vector mLight;  // = new Vector();//平行光源の向き
	
	//--------------------------------------------------------------------
	//コンストラクタ
	//
	//--------------------------------------------------------------------
	public Render(int w,int h)
	{
		mWorld   = new Matrix();
		mMaster  = new Matrix();
		mView    = new Matrix();
		mProj    = new Matrix();
		mLight   = new Vector();
		
		BUFFERW = w;
		BUFFERH = h;
		
		pbuf = new int[BUFFERW * BUFFERH];
		zbuf = new int[BUFFERW * BUFFERH];
		abuf = new int[BUFFERW * BUFFERH];
		
		min  = new SCANLINE[BUFFERH];
		max  = new SCANLINE[BUFFERH];
		
		tvtx = new TLVERTEX[MAX_VERTEX];
		
		for (int i=0;i<BUFFERH;i++){
			min[i] = new SCANLINE();
			max[i] = new SCANLINE();
			min[i].x = +0x7fffffff;
			max[i].x = -0x7fffffff;
		}

		for (int i=0;i<MAX_VERTEX;i++)
			tvtx[i] = new TLVERTEX();
	}

	//--------------------------------------------------------------------
	//バッファのクリア
	//
	//--------------------------------------------------------------------
	static public void clear()
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
	static public void beginScene()
	{
	}
	
	//--------------------------------------------------------------------
	//レンダリング終了
	//
	//Applet applet ...アプレットへの参照
	//Graphics back ...オフスクリーンへの参照
	//--------------------------------------------------------------------
	static public Image endScene(Applet applet)
	{
		return applet.createImage(new MemoryImageSource(BUFFERW,BUFFERH,pbuf,0,BUFFERW));
	}
	*/

	//--------------------------------------------------------------------
	//変換行列を設定する。
	//
	//--------------------------------------------------------------------
	static public void setTransform(int type,Matrix m)
	{
		switch(type){
		case TRANSFORM_WORLD:
			mWorld.Initialize(m);
			break;
		case TRANSFORM_VIEW:
			mView.Initialize(m);
			break;
		case TRANSFORM_PROJ:
			mProj.Initialize(m);
			break;
		}
		
		mMaster.Initialize(mWorld);
		mMaster.Mult(mView);
		mMaster.Mult(mProj);
	}

	//--------------------------------------------------------------------
	//ライト設定
	//
	//--------------------------------------------------------------------
	static public void setLight(Vector l)
	{
		mLight = l;
	}

	//--------------------------------------------------------------------
	//プリミティブ描画
	//
	//--------------------------------------------------------------------
	static public void drawPrimitive(PRIMITIVE pm)
	{			
		VERTEX[] vertices = pm.vertex;
		short[]  indices  = pm.index;
			
		setAttribute(pm);
		setTexture  (pm.texture);
			
		transformAndLighting(vertices);
			
		for (int j=0;j<indices.length;j+=3){
			drawPolygon(tvtx[indices[j+0]],tvtx[indices[j+1]],tvtx[indices[j+2]]);
		}
	}

	//アトリビュート
	static int   mColor;
	static float mSpec;
	static float mDiff;
	static int   mGlos;
	
	//アトリビュート設定
	static private void setAttribute(PRIMITIVE pm)
	{
		mColor = pm.color;
		mSpec  = pm.spec;
		mGlos  = pm.glos;
		mDiff  = pm.diff;
	}

	//テクスチャ設定
	static private void setTexture(TEXTURE tex)
	{
		if (tex == null){
			TEXTUREW = 1;
			TEXTUREH = 1;
			TEXSHIFT = 0;
			TEXMASKW = 0;
			TEXMASKH = 0;
			tbuf     = new int[1];
			tbuf[0] = mColor>>8;
		}else{
			TEXTUREW = tex.w;
			TEXTUREH = tex.h;
			TEXSHIFT = tex.shift;
			TEXMASKW = tex.w - 1;
			TEXMASKH = tex.h - 1;
			tbuf     = tex.img;
		}
	}

	//--------------------------------------------------------------------
	//座標変換、ライティング
	//
	//--------------------------------------------------------------------
	static public void transformAndLighting(VERTEX[] vertices)
	{
		/*
		//ワールド転置行列の作成
		Matrix im = new Matrix();
		im.invert(mWorld);
				
		//平行光源の向き
		Vector light = im.transform(mLight);
		light.normalize();

		Matrix m = mMaster;
		
		for (int i=0;i<vertices.length;i++){
						
			VERTEX vtx = vertices[i];
						
			//座標変換
			float x = m.m00 * vtx.x + m.m10 * vtx.y + m.m20 * vtx.z + m.m30;
			float y = m.m01 * vtx.x + m.m11 * vtx.y + m.m21 * vtx.z + m.m31;
			float w = m.m03 * vtx.x + m.m13 * vtx.y + m.m23 * vtx.z + m.m33;
			float rhw = 1.0f / w;
			
			//光源計算
			float a = light.x * vtx.nx + light.y * vtx.ny + light.z * vtx.nz;			
			int cr = (int)(a * 255) + 64;
			if (cr > 255) cr = 255;
			if (cr <   0) cr =   0;
			cr <<= 16;
			
			//頂点情報の設定と透視変換
			tvtx[i].x = (int)(  x * rhw)<<16;
			tvtx[i].y = (int)( -y * rhw)<<16;
			tvtx[i].z = (int)(rhw * -0x7fffffff);
			tvtx[i].r = cr;
			tvtx[i].g = cr;
			tvtx[i].b = cr;
			tvtx[i].s = 0;
			tvtx[i].u = (int)(vtx.tu * TEXTUREW)<<16;
			tvtx[i].v = (int)(vtx.tv * TEXTUREH)<<16;
		}
		*/
		Matrix m  = mMaster;
		Matrix wm = mWorld;
		Vector n  = new Vector();
		Vector r  = new Vector();
		Matrix im = new Matrix();
		im.Invert(mView);
		Vector e = im.Transform(new Vector(0,0,-1));
		e.Normalize();
		
		Vector re = new Vector();
		
		for (int i=0;i<vertices.length;i++){
						
			VERTEX vtx = vertices[i];
						
			//座標変換
			float x = m.m00 * vtx.x + m.m10 * vtx.y + m.m20 * vtx.z + m.m30;
			float y = m.m01 * vtx.x + m.m11 * vtx.y + m.m21 * vtx.z + m.m31;
			float w = m.m03 * vtx.x + m.m13 * vtx.y + m.m23 * vtx.z + m.m33;
			float rhw = 1.0f / w;
			
			//法線ベクトルのアフィン変換
			n.x = wm.m00 * vtx.nx + wm.m10 * vtx.ny + wm.m20 * vtx.nz;
			n.y = wm.m01 * vtx.nx + wm.m11 * vtx.ny + wm.m21 * vtx.nz;
			n.z = wm.m02 * vtx.nx + wm.m12 * vtx.ny + wm.m22 * vtx.nz;
			n.Normalize();
						
			//ディフューズ計算
			float a = n.DotProduct(mLight);			
			int color = (a > 0? (int)(a * 255) : 0);

			//スペキュラー計算
			r.x = n.x * 2 * a - mLight.x;
			r.y = n.y * 2 * a - mLight.y;
			r.z = n.z * 2 * a - mLight.z;
			
			a = r.DotProduct(e);			
			int spec = (a > 0?  (int)(Math.pow(a,4)*255) : 0);			
			
			//反射視線ベクトルを求める
			a = -n.DotProduct(e) * 2;
			re.x = n.x * a + e.x;
			re.y = n.y * a + e.y;
			re.z = n.z * a + e.z;	
			
			//環境マップに対応するUV座標を求める。			
			re.z += 1;
			float len = 2 * re.Length();
			tvtx[i].u = (int)((re.x / len + 0.5f) * TEXMASKW)<<16;
			tvtx[i].v = (int)((re.y / len + 0.5f) * TEXMASKH)<<16;

			//頂点情報の設定と透視変換
			tvtx[i].x = (int)(  x * rhw)<<16;
			tvtx[i].y = (int)( -y * rhw)<<16;
			tvtx[i].z = (int)(rhw * -0x7fffffff);
			tvtx[i].r = (color + 64)<<16;
			tvtx[i].g = (color + 64)<<16;
			tvtx[i].b = (color + 64)<<16;
			tvtx[i].s = (spec      )<<16;
		}
	}
	
	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static private void drawPolygon(TLVERTEX v1,TLVERTEX v2,TLVERTEX v3)
	{
		//カリング（裏面消去）
		if (((v2.x-v1.x)>>8) * ((v3.y-v1.y)>>8) - ((v2.y-v1.y)>>8) * ((v3.x-v1.x)>>8) <= 0)
			return;

		//スキャン開始位置計算
		int top = +0x7fffffff;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if (top < 0    ) top = 0;
		top >>= 16;
						
		//エッジをスキャン
		scanEdge(v1,v2);
		scanEdge(v2,v3);
		scanEdge(v3,v1);
		
		int offset = top * BUFFERW;

		//ラスタライズ
		for (int y=top;y<BUFFERH;y++,offset+=BUFFERW){
			
			if (min[y].x == +2147483647) break;
						
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
			int z = pmin.z;
			int r = pmin.r;
			int g = pmin.g;
			int b = pmin.b;
			int u = pmin.u;
			int v = pmin.v;
			int s = pmin.s;
			
			//クリッピング
			if (pmin.x < 0){
				int d = -pmin.x;
				z += az * d;
				r += ar * d;
				g += ag * d;
				b += ab * d;
				u += au * d;
				v += av * d;
				s += as * d;
				pmin.x = 0;
			}
			if (pmax.x >= BUFFERW){
				pmax.x  = BUFFERW-1;
			}

			for (int x=pmin.x;x<=pmax.x;x++,z+=az,r+=ar,g+=ag,b+=ab,u+=au,v+=av,s+=as){
							
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] > z){
					//テクセルを取得
					int texel = tbuf[(((v>>16) & TEXMASKH)<<TEXSHIFT) + ((u>>16) & TEXMASKW)];
					
					//テクセルのRGBと頂点色から補完したRGBを乗算
					int tr = (((r>>16) * ((texel & 0xff0000)>>16) )>>8) + (s>>16);
					int tg = (((g>>16) * ((texel & 0x00ff00)>>8 ) )>>8) + (s>>16);
					int tb = (((b>>16) * ((texel & 0x0000ff)    ) )>>8) + (s>>16);
					
					if (tr > 255) tr = 255;
					if (tg > 255) tg = 255;
					if (tb > 255) tb = 255;
					
					pbuf[p] = (tr<<16) | (tg<<8) | tb | 0xff000000;
					zbuf[p] = z;
				}				
			}
			
			//スキャンラインバッファの初期化
			pmin.x = +2147483647;
			pmax.x = -2147483648;
		}
	}

	//--------------------------------------------------------------------
	//エッジをスキャン
	//
	//--------------------------------------------------------------------
	static private void scanEdge(TLVERTEX v1,TLVERTEX v2)
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
