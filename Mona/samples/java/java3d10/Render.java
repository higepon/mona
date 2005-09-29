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
	
	static final int STATE_MIPMAP = 16;	//ミップマップ有効
	static final int STATE_LINER  = 32;	//リニアフィルタ有効
	
	static final int MAX_MIPLEVEL = 5;
	
	static int lv1;
	static int lv2;
	static int blend;
										
	//--------------------------------------------------------------------
	//メンバ
	//
	//--------------------------------------------------------------------
	static /*private*/ int pbuf[];		//32bitピクセルバッファ
	static private int zbuf[];		//32bitＺバッファ
	static private int tbuf[][];	//32bitテクスチャバッファ
	
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
	
	static private int state;

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
		
		tbuf    = new int[MAX_MIPLEVEL][];
		/*
		tbuf[0] = new int[TEXTUREW * TEXTUREH];
		
		PixelGrabber pg = new PixelGrabber(texture,0,0,TEXTUREW,TEXTUREH,tbuf[0],0,TEXTUREW);
				
		try{
			pg.grabPixels();
		}catch(InterruptedException e){}
		*/
		tbuf[0] = texture.data;
		
		CreateMipMap(tbuf,TEXTUREW,TEXTUREH,MAX_MIPLEVEL);
	}

	//--------------------------------------------------------------------
	//領域平均化フィルタを用いてミップマップテクスチャを作成する。
	//
	//--------------------------------------------------------------------
	static final public void CreateMipMap(int[][] tex,int w,int h,int lv)
	{
		for (int i=0;i<lv-1;i++){
			tex[i+1] = new int[w * h / 4];
			int p = 0;
			for (int y=0;y<h;y+=2){
				for (int x=0;x<w;x+=2){
					int rgb1 = tex[i][ y    * w + x    ];
					int rgb2 = tex[i][ y    * w + x + 1];
					int rgb3 = tex[i][(y+1) * w + x    ];
					int rgb4 = tex[i][(y+1) * w + x + 1];
					int r = (((rgb1 & 0xff0000) + (rgb2 & 0xff0000) + (rgb3 & 0xff0000) + (rgb4 & 0xff0000))>>2) & 0xff0000;
					int g = (((rgb1 & 0x00ff00) + (rgb2 & 0x00ff00) + (rgb3 & 0x00ff00) + (rgb4 & 0x00ff00))>>2) & 0x00ff00;
					int b = (((rgb1 & 0x0000ff) + (rgb2 & 0x0000ff) + (rgb3 & 0x0000ff) + (rgb4 & 0x0000ff))>>2) & 0x0000ff;
					tex[i+1][p++] = r | g | b | 0xff000000;
				}
			}
			w /= 2;
			h /= 2; 
		}
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
				pbuf[offset + x] = 0xffffffff;
				zbuf[offset + x] = 0xfffffff;
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
	//レンダリングステートの設定
	//
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
				DrawPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		case PRIMITIVE_POLYGON | STATE_MIPMAP:
			for (int i=0;i<indices.length;i+=3){
				DrawMipPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		case PRIMITIVE_POLYGON | STATE_MIPMAP | STATE_LINER:
			for (int i=0;i<indices.length;i+=3){
				DrawFilteringPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;			
		}
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
		Vector light = im.Transform(new Vector(0,0,1));
		light.Normalize();

		//ワールド・ビュー転置行列の作成
		mMaster.Mult(mView);
		im.Invert(mMaster);

		//視線ベクトル
		Vector eye   = im.Transform( new Vector(0,0,1));
		eye.Normalize();
		eye.Invert();

		//マスター行列の作成
		mMaster.Mult(mProj);
		
		Vector n = new Vector();
		
		for (int i=0;i<vtx.length;i++){
						
			//座標変換
			Vector v = mMaster.Transform(vtx[i].v);
							
			//頂点法線と光源とのなす角
			float w = light.DotProduct(vtx[i].n);
			
			//頂点色を求める
			if (w < 0) w = 0;
			int g = (int)(64 + w * 255)<<16;
						
			//クリッピング（手抜き）
			int active = 1;
			if (v.z < 0) active = 0;
			
			//頂点情報の設定と透視変換
			double _z = 1.0 / v.z;
			tvtx[i]   = new TLVertex();
			tvtx[i].x = (int)( v.x * _z * VIEWPORTW + VIEWPORTCX)<<16;
			tvtx[i].y = (int)(-v.y * _z * VIEWPORTH + VIEWPORTCY)<<16;
			tvtx[i].z = (int)((1.0 - _z) * 0xfffffff);
			tvtx[i].g = g;
			tvtx[i].s = 0;
			tvtx[i].u = (int)(vtx[i].tu * TEXMASKW * 0xffff);
			tvtx[i].v = (int)(vtx[i].tv * TEXMASKH * 0xffff);			
			tvtx[i].active = active;
		}
		
		return tvtx;
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawFilteringPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		if (v1.active + v2.active + v3.active != 3) return;
		
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
		
		//最大最小バッファに基づいて描画する。
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l    = (max [y] - min [y]) + 1;
			int addz = (maxz[y] - minz[y]) / l;
			int addg = (maxg[y] - ming[y]) / l;
			int addu = (maxu[y] - minu[y]) / l;
			int addv = (maxv[y] - minv[y]) / l;
			int adds = (maxs[y] - mins[y]) / l;
			
			//使用するミップマップレベルの決定
			int miplv1 = 0;
			int miplv2 = 0;
			int au = Math.abs(addu>>16);
			if (au < (1<<(MAX_MIPLEVEL-2))){
				for (int i=0;i<MAX_MIPLEVEL;i++)
					if (((au>>i) & 1) > 0) miplv1 = i+1;
			}else{
				miplv1 = MAX_MIPLEVEL - 1;
			}
			if (miplv1 > 0) miplv2 = miplv1 - 1;
			int[] tex1 = tbuf[miplv1];
			int[] tex2 = tbuf[miplv2];
			
			//必要なパラメータの計算
			int VSHIFT1 = 16 + miplv1;
			int USHIFT1 = 16 + miplv1;
			int VMASK1  = TEXMASKH >> miplv1;
			int UMASK1  = TEXMASKW >> miplv1;
			int SHIFT1  = TEXSHIFT-miplv1;
			
			int VSHIFT2 = 16 + miplv2;
			int USHIFT2 = 16 + miplv2;
			int VMASK2  = TEXMASKH >> miplv2;
			int UMASK2  = TEXMASKW >> miplv2;
			int SHIFT2  = TEXSHIFT  - miplv2;
			
			int RATE    = (addu>>(7 + miplv1)) & 0xff;
			int RATE_   = 0xff - RATE;
			
			lv1 = miplv1;
			lv2 = miplv2;
			blend = RATE;
			
			//初期値設定
			int z = minz[y];
			int g = ming[y];
			int u = minu[y];
			int v = minv[y];
			int s = mins[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz,g+=addg,s+=adds,u+=addu,v+=addv){
			
				if (x < VIEWPORTL || x >=VIEWPORTR) continue;
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] >= z){
					//テクセルを取得
					int texel1 = tex1[(((v>>VSHIFT1) & VMASK1)<<SHIFT1) + ((u>>USHIFT1) & UMASK1)];
					int texel2 = tex2[(((v>>VSHIFT2) & VMASK2)<<SHIFT2) + ((u>>USHIFT2) & UMASK2)];					
					
					//２つのミップマップ間を線形補完
					int tr = (((texel1 & 0xff0000)>>16) * RATE + ((texel2 & 0xff0000)>>16) * RATE_)>>8;
					int tg = (((texel1 & 0x00ff00)>>8 ) * RATE + ((texel2 & 0x00ff00)>>8 ) * RATE_)>>8;
					int tb = (((texel1 & 0x0000ff)    ) * RATE + ((texel2 & 0x0000ff)    ) * RATE_)>>8;
					
					//テクセルのRGBと頂点色から補完したRGBを乗算し、スペキュラーを加算
					int _g = g>>16;
					int _s = s>>16;
					
					tr = (( _g * tr )>>8) + _s;
					tg = (( _g * tg )>>8) + _s;
					tb = (( _g * tb )>>8) + _s;
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
	static final private void DrawMipPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		if (v1.active + v2.active + v3.active != 3) return;
		
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
		
		//最大最小バッファに基づいて描画する。
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l    = (max [y] - min [y]) + 1;
			int addz = (maxz[y] - minz[y]) / l;
			int addg = (maxg[y] - ming[y]) / l;
			int addu = (maxu[y] - minu[y]) / l;
			int addv = (maxv[y] - minv[y]) / l;
			int adds = (maxs[y] - mins[y]) / l;
			
			//使用するミップマップレベルの決定
			int miplv1 = 0;
			int au = Math.abs(addu>>16);
			if (au < (1<<(MAX_MIPLEVEL-2))){
				for (int i=0;i<MAX_MIPLEVEL;i++)
					if (((au>>i) & 1) > 0) miplv1 = i+1;
			}else{
				miplv1 = MAX_MIPLEVEL - 1;
			}
			int[] tex = tbuf[miplv1];
			
			//必要なパラメータの計算
			int VSHIFT1 = 16 + miplv1;
			int USHIFT1 = 16 + miplv1;
			int VMASK1  = TEXMASKH >> miplv1;
			int UMASK1  = TEXMASKW >> miplv1;
			int SHIFT1  = TEXSHIFT-miplv1;
			
			//初期値設定
			int z = minz[y];
			int g = ming[y];
			int u = minu[y];
			int v = minv[y];
			int s = mins[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz,g+=addg,s+=adds,u+=addu,v+=addv){
			
				if (x < VIEWPORTL || x >=VIEWPORTR) continue;
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] >= z){
					//テクセルを取得
					int texel = tex[(((v>>VSHIFT1) & VMASK1)<<SHIFT1) + ((u>>USHIFT1) & UMASK1)];
					
					//２つのミップマップ間を線形補完
					int tr = (texel & 0xff0000)>>16;
					int tg = (texel & 0x00ff00)>>8 ;
					int tb = (texel & 0x0000ff)    ;
					
					//テクセルのRGBと頂点色から補完したRGBを乗算し、スペキュラーを加算
					int _g = g>>16;
					int _s = s>>16;
					
					tr = (( _g * tr )>>8) + _s;
					tg = (( _g * tg )>>8) + _s;
					tb = (( _g * tb )>>8) + _s;
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
	static final private void DrawPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		if (v1.active + v2.active + v3.active != 3) return;
		
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
		
		//最大最小バッファに基づいて描画する。
		for (int y=top;y<btm;y++){
			
			int offset = y*BUFFERW;
			
			//増分値計算
			int l    = (max [y] - min [y]) + 1;
			int addz = (maxz[y] - minz[y]) / l;
			int addg = (maxg[y] - ming[y]) / l;
			int addu = (maxu[y] - minu[y]) / l;
			int addv = (maxv[y] - minv[y]) / l;
			int adds = (maxs[y] - mins[y]) / l;
			
			int[] tex = tbuf[0];
			
			//初期値設定
			int z = minz[y];
			int g = ming[y];
			int u = minu[y];
			int v = minv[y];
			int s = mins[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz,g+=addg,s+=adds,u+=addu,v+=addv){
			
				if (x < VIEWPORTL || x >=VIEWPORTR) continue;
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] >= z){
					//テクセルを取得
					int texel = tex[(((v>>16) & TEXMASKH)<<TEXSHIFT) + ((u>>16) & TEXMASKW)];
					
					int tr = (texel & 0xff0000)>>16;
					int tg = (texel & 0x00ff00)>>8 ;
					int tb = (texel & 0x0000ff)    ;
					
					//テクセルのRGBと頂点色から補完したRGBを乗算し、スペキュラーを加算
					int _g = g>>16;
					int _s = s>>16;
					
					tr = (( _g * tr )>>8) + _s;
					tg = (( _g * tg )>>8) + _s;
					tb = (( _g * tb )>>8) + _s;
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
		int l = Math.abs((int)((v2.y>>16) - (v1.y>>16))) + 1;
	
		//増分値計算
		int addx = (v2.x - v1.x) / l;
		int addy = (v2.y - v1.y) / l;
		int addz = (v2.z - v1.z) / l;
		int addg = (v2.g - v1.g) / l;
		int addu = (v2.u - v1.u) / l;
		int addv = (v2.v - v1.v) / l;
		int adds = (v2.s - v1.s) / l;
		
		//初期値設定
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int g = v1.g;
		int u = v1.u;
		int v = v1.v;
		int s = v1.s;
		
		//スキャン
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz,g+=addg,u+=addu,v+=addv,s+=adds){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < VIEWPORTT || py >= VIEWPORTB) continue;			
							
			if (min [py] > px){
				min [py] = px;
				minz[py] = z;
				minu[py] = u;
				minv[py] = v;
				ming[py] = g;
				mins[py] = s;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
				maxu[py] = u;
				maxv[py] = v;
				maxg[py] = g;
				maxs[py] = s;
			}
		}
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
