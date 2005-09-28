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
	static private int tbuf[];		//32bitテクスチャバッファ
	
	static private int BUFFERW;		//バッファ幅
	static private int BUFFERH;		//バッファ高さ
	static private int CENTERX;
	static private int CENTERY;
	static private int TEXTUREW;
	static private int TEXTUREH;
	static private int TEXMASKW;
	static private int TEXMASKH;
	static private int TEXSHIFT;
	
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
	static private int minu[];
	static private int maxu[];
	static private int minv[];
	static /*private*/ int maxv[];
	
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
		minu = new int[BUFFERH];
		maxu = new int[BUFFERH];
		minv = new int[BUFFERH];
		maxv = new int[BUFFERH];
	}

	//--------------------------------------------------------------------
	//テクスチャの設定
	//
	//--------------------------------------------------------------------
	static final public void SetTexture(/*Image*/Texture texture)
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
	//座標変換、ライティング
	//
	//--------------------------------------------------------------------
	static final public TLVertex[] TransformAndLighting(Vertex[] vtx)
	{
		TLVertex[] tvtx = new TLVertex[vtx.length];
		
		//光源色
		Vector a = new Vector(.2f,.2f,.2f);//環境光
		Vector d = new Vector(.9f,.9f,.9f);//拡散光
		
		//転置行列の作成
		Matrix im = new Matrix();
		im.Invert(mMaster);
		
		//平行光源の向き
		Vector light = im.Transform( new Vector(-.2f,.2f,-1f));
		light.Normalize();
		
		//視線ベクトル
		Vector eye   = im.Transform( new Vector(0,0,-1));

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
			tvtx[i]   = new TLVertex();
			tvtx[i].x = (int)(v.x * 0xffff);
			tvtx[i].y = (int)(v.y * 0xffff);
			tvtx[i].z = (int)(v.z * 0xffff);
			tvtx[i].r = r;
			tvtx[i].g = g;
			tvtx[i].b = b;
			
			//テクスチャ座標設定
			tvtx[i].u = (int)(vtx[i].tu * TEXTUREW * 0xffff);
			tvtx[i].v = (int)(vtx[i].tv * TEXTUREH * 0xffff);
			
			float e = eye.DotProduct(vtx[i].n);
			if (e < 0) tvtx[i].active = 0;
			else       tvtx[i].active = 1;
		}
		
		return tvtx;
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{
		if ((v1.active + v2.active + v3.active) == 0) return;
		
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
			int addu = (maxu[y] - minu[y]) / l;
			int addv = (maxv[y] - minv[y]) / l;

			//初期値設定
			int z = minz[y];
			int r = minr[y];
			int g = ming[y];
			int b = minb[y];
			int u = minu[y];
			int v = minv[y];
			
			for (int x=min[y];x<=max[y];x++,z+=addz,r+=addr,g+=addg,b+=addb,u+=addu,v+=addv){
			
				if (x < 0 || x >=BUFFERW) continue;
				
				int p = offset + x;

				//Zバッファとの比較
				if (zbuf[p] > z){
					//テクセルを取得
					int texel = tbuf[(((v>>16) & TEXMASKH)<<TEXSHIFT) + ((u>>16) & TEXMASKW)];
					
					//テクセルのRGBと頂点色から補完したRGBを乗算する。
					int tr = ( (r>>16) * ((texel & 0xff0000)>>16) )>>8;
					int tg = ( (g>>16) * ((texel & 0x00ff00)>>8 ) )>>8;
					int tb = ( (b>>16) * ((texel & 0x0000ff)    ) )>>8;
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
		int addr = (v2.r - v1.r) / l;
		int addg = (v2.g - v1.g) / l;
		int addb = (v2.b - v1.b) / l;
		int addu = (v2.u - v1.u) / l;
		int addv = (v2.v - v1.v) / l;
		
		//初期値設定
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int r = v1.r;
		int g = v1.g;
		int b = v1.b;
		int u = v1.u;
		int v = v1.v;
		
		//スキャン
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz,r+=addr,g+=addg,b+=addb,u+=addu,v+=addv){			
			int py = y>>16;			
			int px = x>>16;
			
			if (py < 0 || py >= BUFFERH) continue;			
							
			if (min [py] > px){
				min [py] = px;
				minz[py] = z;
				minr[py] = r;
				ming[py] = g;
				minb[py] = b;
				minu[py] = u;
				minv[py] = v;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
				maxr[py] = r;
				maxg[py] = g;
				maxb[py] = b;
				maxu[py] = u;
				maxv[py] = v;
			}
		}
	}
}
