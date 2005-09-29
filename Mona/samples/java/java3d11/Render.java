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
	
	static final int STATE_DITHERING = 16;
	
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
	static private int minr[];
	static private int maxr[];
	static private int ming[];
	static private int maxg[];
	static private int minb[];
	static private int maxb[];

	static Matrix mWorld;  //= new Matrix();//ワールド変換行列
	static Matrix mView;   //= new Matrix();//ビュー変換行列
	static Matrix mMaster; //= new Matrix();//マスター行列
	static Matrix mProj;   //= new Matrix();//射影変換行列

	static private int state = 0;
	
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
		minr = new int[BUFFERH];
		maxr = new int[BUFFERH];
		ming = new int[BUFFERH];
		maxg = new int[BUFFERH];
		minb = new int[BUFFERH];
		maxb = new int[BUFFERH];
		
		SetViewPort(0,0,BUFFERW,BUFFERH);
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
				zbuf[offset + x] = 0xffffff;
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
		switch(type){
		case PRIMITIVE_POLYGON:
			for (int i=0;i<indices.length;i+=3){
				DrawPolygon(tvtx[indices[i]],tvtx[indices[i+1]],tvtx[indices[i+2]]);
			}
			break;
		}
	}

	//--------------------------------------------------------------------
	//レンダリングステートの設定
	//
	//--------------------------------------------------------------------
	static final public void SetRenderState(int s)
	{
		state = s;
	}

	//--------------------------------------------------------------------
	//座標変換、ライティング
	//
	//--------------------------------------------------------------------
	static final public TLVertex[] TransformAndLighting(Vertex[] vtx)
	{
		TLVertex[] tvtx = new TLVertex[vtx.length];
		
		//光源色
		Vector a = new Vector(.2f,.2f,.2f);//環境光
		Vector d = new Vector(.5f,.8f,.2f);//拡散光
		
		mMaster.Initialize();
		mMaster.Mult(mWorld);
		
		//転置行列の作成
		Matrix im = new Matrix();
		im.Invert(mMaster);
				
		//平行光源の向き
		Vector light = im.Transform( new Vector(0,0,-1));
		light.Normalize();
	
		mMaster.Mult(mView);
		im.Invert(mMaster);

		//視線ベクトル
		Vector eye   = im.Transform( new Vector(0,0,-1));

		mMaster.Mult(mProj);
		
		int r = 0,g = 0,b = 0;
		
		for (int i=0;i<vtx.length;i++){
						
			//座標変換
			Vector v = mMaster.Transform(vtx[i].v);
							
			//頂点法線と光源とのなす角
			float w = light.DotProduct(vtx[i].n);
			
			//頂点色を求める
			r = (int)((a.x + w * d.x) * 255 * 0xffff);
			g = (int)((a.y + w * d.y) * 255 * 0xffff);
			b = (int)((a.z + w * d.z) * 255 * 0xffff);
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;
			
			//頂点情報の設定
			float _z = 1.0f / v.z;
			tvtx[i]   = new TLVertex();
			tvtx[i].x = (int)( v.x * (VIEWPORTW<<16) * _z + (VIEWPORTCX<<16));
			tvtx[i].y = (int)(-v.y * (VIEWPORTH<<16) * _z + (VIEWPORTCY<<16));
			tvtx[i].z = (int)((1-_z) * 0xffff);
			tvtx[i].r = r;
			tvtx[i].g = g;
			tvtx[i].b = b;
			tvtx[i].active = 1;
		}
		
		return tvtx;
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{		
		int ptn[] = {4,0,2,6};//ディザパターン
		
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
			int addr = (maxr[y] - minr[y]) / l;
			int addg = (maxg[y] - ming[y]) / l;
			int addb = (maxb[y] - minb[y]) / l;

			//初期値設定
			int z = minz[y];
			int r = minr[y];
			int g = ming[y];
			int b = minb[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz,r+=addr,g+=addg,b+=addb){
			
				if (x < VIEWPORTL || x >=VIEWPORTR) continue;
				
				int p = offset + x;

				if (zbuf[p] > z){
					int tr = r>>16;
					int tg = g>>16;
					int tb = b>>16;
					if (tr > 255) tr = 254;
					if (tg > 255) tg = 254;
					if (tb > 255) tb = 254;
					
					//ディザリング（パターンディザ）
					if ((state & STATE_DITHERING) > 0){
						int pp = ptn[((y & 1) << 1) + (x & 1)];
						if ((tr & 7) >= pp) tr += 8;
						if ((tg & 7) >= pp) tg += 8;
						if ((tb & 7) >= pp) tb += 8;
					}
					
					//To16bitColor
					tr = tr>>3<<3;
					tg = tg>>3<<3;
					tb = tb>>3<<3;
					
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
		int addr = (v2.r - v1.r) / l;
		int addg = (v2.g - v1.g) / l;
		int addb = (v2.b - v1.b) / l;
		
		//初期値設定
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int r = v1.r;
		int g = v1.g;
		int b = v1.b;
		
		//スキャン
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz,r+=addr,g+=addg,b+=addb){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < VIEWPORTT || py >= VIEWPORTB) continue;			
							
			if (min [py] > px){
				min [py] = px;
				minz[py] = z;
				minr[py] = r;
				ming[py] = g;
				minb[py] = b;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
				maxr[py] = r;
				maxg[py] = g;
				maxb[py] = b;
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
