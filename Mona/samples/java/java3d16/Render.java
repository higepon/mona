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
	//--------------------------------------------------------------------
	//フィールド
	//
	//--------------------------------------------------------------------
	static final int PRIMITIVE_POLYGON = 1;
		
	static final int TRANSFORM_WORLD = 0;
	static final int TRANSFORM_VIEW  = 1;
	static final int TRANSFORM_PROJ  = 2;
	
	static final int STATE_ALPHABLEND = 16;
	static final int STATE_SHADOW     = 32;
									
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ int pbuf[];		//32bitピクセルバッファ
	static private int zbuf[];		//32bitＺバッファ
	static private int tbuf[];		//32bitテクスチャバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
	static private int CENTERX;
	static private int CENTERY;
	static private int TEXTUREW;	//テクスチャ
	static private int TEXTUREH;
	static private int TEXMASKW;
	static private int TEXMASKH;
	static private int TEXSHIFT;
	
	static private int VIEWPORTL;	//ビューポート
	static private int VIEWPORTR;
	static private int VIEWPORTT;
	static private int VIEWPORTB;
	static private int VIEWPORTW;
	static private int VIEWPORTH;
	static private int VIEWPORTCX;
	static private int VIEWPORTCY;
	
	static private int min [];
	static private int max [];
	static private int minz[];
	static private int maxz[];
	static private int ming[];
	static private int maxg[];
	static private int minu[];
	static private int maxu[];
	static private int minv[];
	static private int maxv[];
	static private int mins[];
	static private int maxs[];

	static Matrix mWorld;  //= new Matrix();//ワールド変換行列
	static Matrix mView;   //= new Matrix();//ビュー変換行列
	static Matrix mMaster; //= new Matrix();//マスター行列
	static Matrix mProj;   //= new Matrix();//射影変換行列

	static int state   = 0;
	static float lightlv = 0;
	
	//--------------------------------------------------------------------
	//コンストラクタ
	//
	//--------------------------------------------------------------------
	public Render(int w,int h)
	{
		mWorld  = new Matrix();
		mView   = new Matrix();
		mMaster = new Matrix();
		mProj   = new Matrix();
		
		BUFFERW = w;
		BUFFERH = h;
		CENTERX = BUFFERW/2;
		CENTERY = BUFFERH/2;
		
		pbuf = new int[BUFFERW * BUFFERH];
		zbuf = new int[BUFFERW * BUFFERH];
		min  = new int[BUFFERH];
		max  = new int[BUFFERH];
		minz = new int[BUFFERH];
		maxz = new int[BUFFERH];
		ming = new int[BUFFERH];
		maxg = new int[BUFFERH];
		minu = new int[BUFFERH];
		maxu = new int[BUFFERH];
		minv = new int[BUFFERH];
		maxv = new int[BUFFERH];
		mins = new int[BUFFERH];
		maxs = new int[BUFFERH];
		
		SetViewPort(0,0,BUFFERW,BUFFERH);
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
		for (int y=VIEWPORTT;y<VIEWPORTB;y++){
			int offset = y * BUFFERW;
			for (int x=VIEWPORTL;x<VIEWPORTR;x++){
				pbuf[offset + x] = 0xff000000;
				zbuf[offset + x] = 2147483647;
			}
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
	//レンダリングステートを設定する。
	//
	//int s..状態
	//--------------------------------------------------------------------
	static final public void SetRenderState(int s)
	{
		state = s;
	}
	
	//--------------------------------------------------------------------
	//変換行列を設定する。
	//
	//int type ...行列の種類
	//Matrix m ...変換行列
	//--------------------------------------------------------------------
	static final public void SetTransform(int type,Matrix m)
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
	}

	//--------------------------------------------------------------------
	//プリミティブ描画
	//
	//--------------------------------------------------------------------
	static final public void DrawIndexedPrimitive(int type,Vertex[] vertices,int[] indices)
	{
		//座標変換、ライティング
		TLVertex[] tvtx = TransformAndLighting(vertices);
		
		//ラスタライズ
		switch(type | state){
		case PRIMITIVE_POLYGON:
			for (int i=0;i<indices.length;i+=3){
				Vector v1 = vertices[indices[i  ]].v_;
				Vector v2 = vertices[indices[i+1]].v_;
				Vector v3 = vertices[indices[i+2]].v_;
				Vector  n = CalcNormal(v1,v2,v3);
				if (n.DotProduct(v1) < 0) continue;
				DrawPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		case PRIMITIVE_POLYGON | STATE_ALPHABLEND:
			for (int i=0;i<indices.length;i+=3){
				Vector v1 = vertices[indices[i  ]].v_;
				Vector v2 = vertices[indices[i+1]].v_;
				Vector v3 = vertices[indices[i+2]].v_;
				Vector  n = CalcNormal(v1,v2,v3);
				if (n.DotProduct(v1) < 0) continue;
				DrawAlphaPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		case PRIMITIVE_POLYGON | STATE_SHADOW:
			for (int i=0;i<indices.length;i+=3){
				Vector v1 = vertices[indices[i  ]].v_;
				Vector v2 = vertices[indices[i+1]].v_;
				Vector v3 = vertices[indices[i+2]].v_;
				Vector  n = CalcNormal(v1,v2,v3);
				if (n.DotProduct(v1) < 0) continue;
				DrawShadowPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
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
		n.Invert();
		
		return n;
	}

	//--------------------------------------------------------------------
	//座標変換、ライティング
	//
	//--------------------------------------------------------------------
	static final public TLVertex[] TransformAndLighting(Vertex[] vtx)
	{
		TLVertex[] tvtx = new TLVertex[vtx.length];
				
		mMaster.Initialize();
		mMaster.Mult(mWorld);
		
		//ワールド転置行列の作成
		Matrix im = new Matrix();
		im.Invert(mMaster);
				
		//平行光源の向き
		Vector light = new Vector(.5f,.8f,0);
		light.Normalize();
		
		Vector ilight = im.Transform(light);
		ilight.Normalize();

		//マスター行列の作成
		mMaster.Mult(mView);
		mMaster.Mult(mProj);
		
		Vector n = new Vector();
		
		float mw = 0xffff * VIEWPORTW;
		float mh = 0xffff * VIEWPORTH;
		float tw = 0xffff * TEXTUREW;
		float th = 0xffff * TEXTUREH;
		
		for (int i=0;i<vtx.length;i++){
						
			//座標変換
			Vector v = vtx[i].v_ = mMaster.Transform(vtx[i].v);
							
			//頂点法線と光源とのなす角
			float w = (ilight.DotProduct(vtx[i].n) + 1)/2;
			
			//頂点色を求める
			int g = (int)((32 + w * 225) * 0xffff);
			if (g < (32<<16)) g = (32<<16);
				
			int active = 1;
			
			//頂点情報の設定と透視変換
			float _z = 1 / v.z;
			tvtx[i]   = new TLVertex();
			tvtx[i].x = (int)( v.x * mw * _z) + (VIEWPORTCX<<16);
			tvtx[i].y = (int)(-v.y * mh * _z) + (VIEWPORTCY<<16);
			tvtx[i].z = (int)( v.z * 0xffff);
			tvtx[i].g = (int)(g<<1);
			tvtx[i].s = 0;
			tvtx[i].u = (int)(vtx[i].tu * tw);
			tvtx[i].v = (int)(vtx[i].tv * th);			
			tvtx[i].active = active;
		}
		
		return tvtx;
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		//バッファの使用範囲を設定（高速化のため）
		int top = +2147483647;
		int btm = -2147483648;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if (btm < v1.y ) btm = v1.y;
		if (btm < v2.y ) btm = v2.y;
		if (btm < v3.y ) btm = v3.y;
		top>>=16;
		btm>>=16;
		if (top < VIEWPORTT) top = VIEWPORTT;
		if (btm > VIEWPORTB) btm = VIEWPORTB;
				
		//最大最小バッファの初期化
		for (int i=top;i<btm;i++){
			min[i] = +2147483647;
			max[i] = -2147483648;
		}
		
		//スキャンエッジ
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v1);
		
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l = (max [y] - min [y]);
			if (l==0) continue;

			int _l = 0x7fffffff / l;
			int addz = (int)((long)(maxz[y] - minz[y]) * _l >> 31);
			int addg = (int)((long)(maxg[y] - ming[y]) * _l >> 31);
			int addu = (int)((long)(maxu[y] - minu[y]) * _l >> 31);
			int addv = (int)((long)(maxv[y] - minv[y]) * _l >> 31);

			//クリッピング
			if (min [y] <  VIEWPORTL){
				int c    = VIEWPORTL-min[y];
				ming[y] += addg * c; 
				minu[y] += addu * c; 
				minv[y] += addv * c; 
				min [y]  = VIEWPORTL; 
			}

			if (max [y] >= VIEWPORTR){
				max [y]  = VIEWPORTR - 1; 
			}

			//初期値設定
			int z = minz[y];
			int g = ming[y];
			int u = minu[y];
			int v = minv[y];
			
			for (int x=min[y];x<max[y];x++,z+=addz,g+=addg,u+=addu,v+=addv){
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] >= z){
					int texel = 0xff404080;

					int _g = g>>16;
						
					int tr = (( _g * ((texel & 0xff0000)>>16) )>>8);
					int tg = (( _g * ((texel & 0x00ff00)>>8 ) )>>8);
					int tb = (( _g * ((texel & 0x0000ff)    ) )>>8);
					
					if (tr > 255) tr = 255;
					if (tg > 255) tg = 255;
					if (tb > 255) tb = 255;
					
					pbuf[p] = (tr<<16) | (tg<<8) | tb | 0xff000000;
					zbuf[p] = z;
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawShadowPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		//バッファの使用範囲を設定（高速化のため）
		int top = +2147483647;
		int btm = -2147483648;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if (btm < v1.y ) btm = v1.y;
		if (btm < v2.y ) btm = v2.y;
		if (btm < v3.y ) btm = v3.y;
		top>>=16;
		btm>>=16;
		if (top < VIEWPORTT) top = VIEWPORTT;
		if (btm > VIEWPORTB) btm = VIEWPORTB;
				
		//最大最小バッファの初期化
		for (int i=top;i<btm;i++){
			min[i] = +2147483647;
			max[i] = -2147483648;
		}
		
		//スキャンエッジ
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v1);
		
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l = (max [y] - min [y]);
			if (l==0) continue;

			int _l = 0x7fffffff / l;
			int addz = (int)((long)(maxz[y] - minz[y]) * _l >> 31);

			//クリッピング
			if (min [y] <  VIEWPORTL){
				min [y]  = VIEWPORTL; 
			}

			if (max [y] >= VIEWPORTR){
				max [y]  = VIEWPORTR - 1; 
			}

			//初期値設定
			int z = minz[y];
			
			for (int x=min[y];x<max[y];x++,z+=addz){
				
				int p = offset + x;

				int pixel = pbuf[p];
										
				int tr = (pixel&0xff0000)*3>>18;
				int tg = (pixel&0x00ff00)*3>>10;
				int tb = (pixel&0x0000ff)*3>> 2;
									
				pbuf[p] = (tr<<16) | (tg<<8) | tb | 0xff000000;
				zbuf[p] = z;

			}
		}
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawAlphaPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		//バッファの使用範囲を設定（高速化のため）
		int top = +2147483647;
		int btm = -2147483648;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if (btm < v1.y ) btm = v1.y;
		if (btm < v2.y ) btm = v2.y;
		if (btm < v3.y ) btm = v3.y;
		top>>=16;
		btm>>=16;
		if (top < VIEWPORTT) top = VIEWPORTT;
		if (btm > VIEWPORTB) btm = VIEWPORTB;
				
		//最大最小バッファの初期化
		for (int i=top;i<btm;i++){
			min[i] = +2147483647;
			max[i] = -2147483648;
		}
		
		//スキャンエッジ
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v1);
		
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l = (max [y] - min [y]);
			if (l==0) l = 1;

			int _l = 0x7fffffff / l;
			int addz = (int)((long)(maxz[y] - minz[y]) * _l >> 31);
			int addg = (int)((long)(maxg[y] - ming[y]) * _l >> 31);
			int addu = (int)((long)(maxu[y] - minu[y]) * _l >> 31);
			int addv = (int)((long)(maxv[y] - minv[y]) * _l >> 31);

			//クリッピング
			if (min [y] <  VIEWPORTL){
				int c    = VIEWPORTL-min[y];
				ming[y] += addg * c; 
				minu[y] += addu * c; 
				minv[y] += addv * c; 
				min [y]  = VIEWPORTL; 
			}

			if (max [y] >= VIEWPORTR){
				max [y]  = VIEWPORTR - 1; 
			}

			//初期値設定
			int z = minz[y];
			int g = ming[y];
			int u = minu[y];
			int v = minv[y];
			
			for (int x=min[y];x<max[y];x++,z+=addz,g+=addg,u+=addu,v+=addv){
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] >= z){
					//テクセルを取得
					int texel = GetTexelBiLiner(u,v);
//					int texel = GetTexel(u,v);

					int pixel = pbuf[p];
						
					int tr = (texel & 0xff0000)>>16;
					int tg = (texel & 0x00ff00)>>8 ;
					int tb = (texel & 0x0000ff)    ;
					
					tr = ((((pixel&0xff0000)>>16) - tr) * 64 >> 8) + tr;
					tg = ((((pixel&0x00ff00)>> 8) - tg) * 64 >> 8) + tg;
					tb = ((((pixel&0x0000ff)    ) - tb) * 64 >> 8) + tb;
					
					if (tr > 255) tr = 255;
					if (tg > 255) tg = 255;
					if (tb > 255) tb = 255;
					
					pbuf[p] = (tr<<16) | (tg<<8) | tb | 0xff000000;
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
	static final private void ScanEdge(TLVertex v1,TLVertex v2)
	{		
		int l = Math.abs((v2.y>>16) - (v1.y>>16));
		if (l==0) l=1;

		//増分値計算
		int _l = 0x7fffffff / l;
		int addx = (int)((long)(v2.x - v1.x) * _l >> 31);
		int addy = (int)((long)(v2.y - v1.y) * _l >> 31);
		int addz = (int)((long)(v2.z - v1.z) * _l >> 31);
		int addg = (int)((long)(v2.g - v1.g) * _l >> 31);
		int addu = (int)((long)(v2.u - v1.u) * _l >> 31);
		int addv = (int)((long)(v2.v - v1.v) * _l >> 31);
		
		//初期値設定
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int g = v1.g;
		int u = v1.u;
		int v = v1.v;
		
		//スキャン
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz,g+=addg,u+=addu,v+=addv){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < VIEWPORTT || py >= VIEWPORTB) continue;
		
			if (min [py] > px){
				min [py] = px;
				minz[py] = z;
				minu[py] = u;
				minv[py] = v;
				ming[py] = g;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
				maxu[py] = u;
				maxv[py] = v;
				maxg[py] = g;
			}
		}
	}

	//--------------------------------------------------------------------
	//テクセルを取得
	//
	//int u ...16bitの少数部をもつＵ座標
	//int v ...16bitの少数部をもつＶ座標
	//--------------------------------------------------------------------
	static public int GetTexel(int u,int v)
	{
		return tbuf[(((v>>16) & TEXMASKH)<<TEXSHIFT) + ((u>>16) & TEXMASKW)];
	}

	//--------------------------------------------------------------------
	//テクセルを取得（バイリニアフィルタリング）
	//
	//int u ...16bitの少数部をもつＵ座標
	//int v ...16bitの少数部をもつＶ座標
	//--------------------------------------------------------------------
	static public int GetTexelBiLiner(int u,int v)
	{
		int u_ = u>>16;
		int v_ = v>>16;
		int texel1 = tbuf[(((v_  ) & TEXMASKH)<<TEXSHIFT) + ((u_  ) & TEXMASKW)];
		int texel2 = tbuf[(((v_  ) & TEXMASKH)<<TEXSHIFT) + ((u_+1) & TEXMASKW)];
		int texel3 = tbuf[(((v_+1) & TEXMASKH)<<TEXSHIFT) + ((u_  ) & TEXMASKW)];
		int texel4 = tbuf[(((v_+1) & TEXMASKH)<<TEXSHIFT) + ((u_+1) & TEXMASKW)];
					
		int tmp;
		int ru = (u>>8) & 0xff;//Ｕブレンディングレート(0-255)
		int rv = (v>>8) & 0xff;//Ｖブレンディングレート(0-255)
					
		//線形補完 (a - b) * rate + b 
		//1-2間
		tmp = (texel1 & 0xff0000)>>16;
		int tr12 = ((((texel2 & 0xff0000)>>16) - tmp) * ru >>8) + tmp;
		tmp = (texel1 & 0x00ff00)>>8 ;
		int tg12 = ((((texel2 & 0x00ff00)>>8 ) - tmp) * ru >>8) + tmp;
		tmp = (texel1 & 0x0000ff)    ;
		int tb12 = ((((texel2 & 0x0000ff)    ) - tmp) * ru >>8) + tmp;
		//3-4間
		tmp = (texel3 & 0xff0000)>>16;
		int tr34 = ((((texel4 & 0xff0000)>>16) - tmp) * ru >>8) + tmp;
		tmp = (texel3 & 0x00ff00)>>8 ;
		int tg34 = ((((texel4 & 0x00ff00)>>8 ) - tmp) * ru >>8) + tmp;
		tmp = (texel3 & 0x0000ff)    ;
		int tb34 = ((((texel4 & 0x0000ff)    ) - tmp) * ru >>8) + tmp;
		//12-34間
		int tr = ((tr34 - tr12) * rv >>8) + tr12;
		int tg = ((tg34 - tg12) * rv >>8) + tg12;
		int tb = ((tb34 - tb12) * rv >>8) + tb12;
		
		return (tr<<16) | (tg<<8) | tb | 0xff000000;
	}

	//--------------------------------------------------------------------
	//ビューポートの設定
	//
	//int x ...左上座標
	//int y ...
	//int w ...幅と高さ
	//--------------------------------------------------------------------
	static public void SetViewPort(int x,int y,int w,int h)
	{
		VIEWPORTW  = w/2;
		VIEWPORTH  = h/2;
		VIEWPORTL  = x;
		VIEWPORTR  = x + w;
		VIEWPORTT  = y;
		VIEWPORTB  = y + h;
		VIEWPORTCX = x + w/2;
		VIEWPORTCY = y + h/2;
	}
}
