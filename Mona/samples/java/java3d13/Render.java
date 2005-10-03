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
	static final int PRIMITIVE_3DLINE  = 0;
	static final int PRIMITIVE_POLYGON = 1;
	
	static final int TRANSFORM_WORLD    = 0;
	static final int TRANSFORM_VIEW     = 1;
	static final int TRANSFORM_PROJ     = 2;
	static final int TRANSFORM_CLIP     = 3;
	static final int TRANSFORM_VIEWPORT = 4;
	
	static Matrix mMaster;   //= new Matrix();
	static Matrix mWorld;    //= new Matrix();
	static Matrix mView;     //= new Matrix();
	static Matrix mProj;     //= new Matrix();
	static Matrix mClip;     //= new Matrix();
	static Matrix mViewPort; //= new Matrix();
										 
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
	static private int maxv[];
	static private int minsr[];
	static private int maxsr[];
	static private int minsg[];
	static private int maxsg[];
	static private int minsb[];
	static private int maxsb[];
	
	static private TLVertex[] tvtx;
	
	//--------------------------------------------------------------------
	//コンストラクタ
	//
	//--------------------------------------------------------------------
	public Render(int w,int h)
	{
		mMaster   = new Matrix();
		mWorld    = new Matrix();
		mView     = new Matrix();
		mProj     = new Matrix();
		mClip     = new Matrix();
		mViewPort = new Matrix();
		
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
		minsr= new int[BUFFERH];
		maxsr= new int[BUFFERH];
		minsg= new int[BUFFERH];
		maxsg= new int[BUFFERH];
		minsb= new int[BUFFERH];
		maxsb= new int[BUFFERH];
		
		tvtx = new TLVertex[4096];
		
		for (int i=0;i<tvtx.length;i++)
			tvtx[i] = new TLVertex();
		
		//最大最小バッファの初期化
		for (int i=0;i<BUFFERH;i++){
			min[i] = +0x7fffffff;
			max[i] = -0x7fffffff;
		}
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
		case TRANSFORM_CLIP:
			mClip.Initialize(m);
			break;
		case TRANSFORM_VIEWPORT:
			mViewPort.Initialize(m);
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
		TransformAndLighting(vertices);
		
		//ラスタライズ
		switch(type){
		case PRIMITIVE_3DLINE:
			for (int i=0;i<indices.length;i+=3){
				Draw3DLine(tvtx[indices[i  ]],tvtx[indices[i+1]]);
				Draw3DLine(tvtx[indices[i+1]],tvtx[indices[i+2]]);
				Draw3DLine(tvtx[indices[i+2]],tvtx[indices[i  ]]);
			}
			break;
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
	static final public void TransformAndLighting(Vertex[] vtx)
	{
		mMaster.Initialize();
		mMaster.Mult(mWorld);
		
		//転置行列の作成
		Matrix im = new Matrix();
		im.Invert(mMaster);
		
		//平行光源の向き
		Vector light = im.Transform(new Vector(1,2,1.5f));
		light.Normalize();
		
		mMaster.Mult(mView);		
		im.Invert(mMaster);
		//視線ベクトル
		Vector eye   = im.Transform( new Vector(0,0,1));
		eye.Normalize();
		
		mMaster.Mult(mProj);
		mMaster.Mult(mClip);
		mMaster.Mult(mViewPort);

		Vector n = new Vector();

		int r,g,b,sr,sg,sb;
		
		for (int i=0;i<vtx.length;i++){
						
			//座標変換
			Vector v = vtx[i].v;
			float sx = v.x * mMaster.m00 + v.y * mMaster.m10 + v.z * mMaster.m20 + mMaster.m30;
			float sy = v.x * mMaster.m01 + v.y * mMaster.m11 + v.z * mMaster.m21 + mMaster.m31;
			float sz = v.x * mMaster.m02 + v.y * mMaster.m12 + v.z * mMaster.m22 + mMaster.m32;
			float w  = v.x * mMaster.m03 + v.y * mMaster.m13 + v.z * mMaster.m23 + mMaster.m33;
			float rhw= 1/w;
							
			//頂点法線と光源とのなす角
			float a = light.DotProduct(vtx[i].n);
						
			//スペキュラーを求める
			n.x = vtx[i].n.x * 2 * a - light.x;
			n.y = vtx[i].n.y * 2 * a - light.y;
			n.z = vtx[i].n.z * 2 * a - light.z;
			n.Normalize();
						
			float sa = n.DotProduct(eye);//視線ベクトルと反射ベクトルとの内積
			if (sa < 0) sa = 0;
			sa = (float)Math.pow(sa,12);
			
			a = (a+1)/2;
																
			tvtx[i].x = (int)( sx * rhw * 0xffff);
			tvtx[i].y = (int)(-sy * rhw * 0xffff);
			tvtx[i].z = (int)(-rhw * 0xffffff);
			tvtx[i].r = (int)(32 + a * 0xaa)<<16;
			tvtx[i].g = (int)(32 + a * 0xaa)<<16;
			tvtx[i].b = (int)(80 + a * 0xff)<<16;
			tvtx[i].sr= (int)(sa * 0xcc)<<16;
			tvtx[i].sg= (int)(sa * 0xcc)<<16;
			tvtx[i].sb= (int)(sa * 0xff)<<16;
			
			//テクスチャ座標設定
			tvtx[i].u = (int)(vtx[i].tu * TEXTUREW)<<16;
			tvtx[i].v = (int)(vtx[i].tv * TEXTUREH)<<16;
			
			if (eye.DotProduct(vtx[i].n) < 0) tvtx[i].active = 0;
			else                              tvtx[i].active = 1;
		}
	}

	//--------------------------------------------------------------------
	//ポリゴン描画
	//
	//--------------------------------------------------------------------
	static final private void DrawPolygon(TLVertex v1,TLVertex v2,TLVertex v3)
	{
		if ((v1.active + v2.active + v3.active) == 0) return;
		
		//バッファの使用範囲を設定（高速化のため）
		int top = +0x7fffffff;
		if (top > v1.y ) top = v1.y;
		if (top > v2.y ) top = v2.y;
		if (top > v3.y ) top = v3.y;
		if ((top>>=16) < 0) top = 0;
						
		//スキャンエッジ
		ScanEdge(v1,v2);
		ScanEdge(v2,v3);
		ScanEdge(v3,v1);

		int offset = top * BUFFERW;
		for (int y=top;y<BUFFERH;y++,offset+=BUFFERW){
			
			if (min[y] == 0x7fffffff) break;
						
			//増分値計算
			int l = (max[y] - min[y]);
			int rhl  = (l == 0? 0x7fffffff : 0x7fffffff / l);
			int addz = (int)((long)(maxz[y] - minz[y]) * rhl >> 31);
			int addr = (int)((long)(maxr[y] - minr[y]) * rhl >> 31);
			int addg = (int)((long)(maxg[y] - ming[y]) * rhl >> 31);
			int addb = (int)((long)(maxb[y] - minb[y]) * rhl >> 31);
			int addu = (int)((long)(maxu[y] - minu[y]) * rhl >> 31);
			int addv = (int)((long)(maxv[y] - minv[y]) * rhl >> 31);
			int addsr= (int)((long)(maxsr[y]-minsr[y]) * rhl >> 31);
			int addsg= (int)((long)(maxsg[y]-minsg[y]) * rhl >> 31);
			int addsb= (int)((long)(maxsb[y]-minsb[y]) * rhl >> 31);
			
			//初期値設定
			int z = minz[y];
			int r = minr[y];
			int g = ming[y];
			int b = minb[y];
			int u = minu[y];
			int v = minv[y];
			int sr= minsr[y];
			int sg= minsg[y];
			int sb= minsb[y];

			//クリッピング
			if (min[y] < 0){
				int c = -min[y];
				z += addz * c;
				r += addr * c;
				g += addg * c;
				b += addb * c;
				sr+= addsr* c;
				sg+= addsg* c;
				sb+= addsb* c;
				u += addu * c;
				v += addv * c;
				min[y] = 0;
			}
			if (max[y] >= BUFFERW){
				max[y]  = BUFFERW-1;
			}

			for (int x=min[y];x<max[y];x++,z+=addz,r+=addr,g+=addg,b+=addb,u+=addu,v+=addv,sr+=addsr,sg+=addsg,sb+=addsb){
				
				int p = offset + x;

				if (zbuf[p] >= z){
					int texel = tbuf[(((v>>16) & TEXMASKH)<<TEXSHIFT) + ((u>>16) & TEXMASKW)];
					
					int tr = (( (r>>16) * ((texel & 0xff0000)>>16) )>>8) + (sr>>16);
					int tg = (( (g>>16) * ((texel & 0x00ff00)>>8 ) )>>8) + (sg>>16);
					int tb = (( (b>>16) * ((texel & 0x0000ff)    ) )>>8) + (sb>>16);
					if (tr > 255) tr = 255;
					if (tg > 255) tg = 255;
					if (tb > 255) tb = 255;
					
					pbuf[p] = (tr<<16) | (tg<<8) | tb | 0xff000000;
					zbuf[p] = z;
				}
			}
			min[y] = +0x7fffffff;
			max[y] = -0x7fffffff;
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
		if (l==0) l = 1;
		
		int rhl  = 0x7fffffff / l;
		int addx = (int)((long)(v2.x - v1.x) * rhl >> 31);
		int addy = (int)((long)(v2.y - v1.y) * rhl >> 31);
		int addz = (int)((long)(v2.z - v1.z) * rhl >> 31);
		int addr = (int)((long)(v2.r - v1.r) * rhl >> 31);
		int addg = (int)((long)(v2.g - v1.g) * rhl >> 31);
		int addb = (int)((long)(v2.b - v1.b) * rhl >> 31);
		int addu = (int)((long)(v2.u - v1.u) * rhl >> 31);
		int addv = (int)((long)(v2.v - v1.v) * rhl >> 31);
		int addsr= (int)((long)(v2.sr-v1.sr) * rhl >> 31);
		int addsg= (int)((long)(v2.sg-v1.sg) * rhl >> 31);
		int addsb= (int)((long)(v2.sb-v1.sb) * rhl >> 31);

		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		int r = v1.r;
		int g = v1.g;
		int b = v1.b;
		int u = v1.u;
		int v = v1.v;
		int sr= v1.sr;
		int sg= v1.sg;
		int sb= v1.sb;
		
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz,r+=addr,g+=addg,b+=addb,u+=addu,v+=addv,sr+=addsr,sg+=addsg,sb+=addsb){
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
				minsr[py]= sr;
				minsg[py]= sg;
				minsb[py]= sb;
			}
			
			if (max [py] < px){
				max [py] = px;
				maxz[py] = z;
				maxr[py] = r;
				maxg[py] = g;
				maxb[py] = b;
				maxu[py] = u;
				maxv[py] = v;
				maxsr[py]= sr;
				maxsg[py]= sg;
				maxsb[py]= sb;
			}
		}
	}
	
	//--------------------------------------------------------------------
	//３Ｄライン描画
	//
	//Vector v1 ...始点
	//Vector v2 ...終点
	//--------------------------------------------------------------------
	static final private void Draw3DLine(TLVertex v1,TLVertex v2)
	{		
		int lx = Math.abs((v2.x>>16) - (v1.x>>16));
		int ly = Math.abs((v2.y>>16) - (v1.y>>16));
		int l  = (lx > ly? lx : ly);
		if (l==0) l = 1;
		
		int addx = (v2.x - v1.x) / l;
		int addy = (v2.y - v1.y) / l;
		int addz = (v2.z - v1.z) / l;
		int x = v1.x;
		int y = v1.y;
		int z = v1.z;
		
		for (int i=0;i<l;i++,x+=addx,y+=addy,z+=addz){
			int py = y>>16;
			int px = x>>16;
			
			if (py < 0 || py >= BUFFERH) continue;			
			if (px < 0 || px >= BUFFERW) continue;

			int p = py * BUFFERW + px;
			if (zbuf[p] >= z){
				pbuf[p] = 0xff0000ff;
				zbuf[p] = z;
			}
		}
	}
}
