/*
import java.awt.*;
import java.applet.*;
*/

public class Applet1 //extends Applet implements Runnable
{
	/*
	Thread   thread;	//スレッド
	Graphics back;		//バックバッファ
	Image    offimage;
	Image    texture;	//テクスチャ
	*/
	Texture texture;
		
	static final int SCREENW     = 320;	//スクリーン高さ
	static final int SCREENH     = 200;	//スクリーン幅
	static final int FRAMERATE   = 10;	//フレームレート
	static final int DETAILLEVEL = 24;	//モデルの細かさ
	
	float rx;
	float ry;
	float rz;
	
	boolean flag = true;
	

	//オブジェクト
	Vertex    vertices[];	//頂点配列
	int       indices [];	//インデックス配列

	//バウンディングオブジェクト
	Vertex    bvertices[];	//頂点配列
	int       bindices [];	//インデックス配列

	Render render = new Render(SCREENW,SCREENH);
	//----------------------------------------------------------
	//アプレットのお約束処理
	//
	//init
	//start
	//update
	//paint
	//stop
	//----------------------------------------------------------
	public void init()
	{
		//オフスクリーンバッファの作成
		/*
		offimage  = createImage(SCREENW,SCREENH);
		back      = offimage.getGraphics();
		*/
		Render.Clear();

		//テクスチャのロード
		/*
		texture   = getImage(getCodeBase(),"texture.gif");
		
		MediaTracker tracker = new MediaTracker(this);
		tracker.addImage(texture,0);
		try{
			tracker.waitForAll();
		}catch(Exception e){}
		*/
		texture = new Texture();
		
		//テクスチャの設定
		render.SetTexture(texture);

		//頂点データの作成
		SimpleShape ss = new SimpleShape();

		ss.CreateDonuts(DETAILLEVEL);
		
		//頂点リストを取得
		vertices = ss.GetVB();
		indices  = ss.GetIndices();
		
		//バウンディングオブジェクトの作成
		ss.CreateBoundingSphere(vertices,16);
		
		bvertices = ss.GetVB();
		bindices  = ss.GetIndices();
	}

	/*
	public void start()
	{
		thread = new Thread(this);
		if (thread!=null) thread.start();
	}
	
	public void paint(Graphics g)
	{
		g.drawImage(offimage,0,0,this);
	}
	
	public void update(Graphics g)
	{	
		MainLoop();
		FrameRate(back);
			
		paint(g);
	}
	*/

	public void run(){
		/*
		int   count = 0;
		float fps   = 0; 
		long  time  = 0;

		while(true){			
			repaint();
			try{
				Thread.sleep(FRAMERATE);
			}catch(Exception e){}
		}
		*/
		SimpleCanvas.create(render.pbuf,"Specular",SCREENW,SCREENH);
		while (SimpleCanvas.doEvents()) {
			MainLoop();
			SimpleCanvas.invalidate();
		}
	}

	public void stop(){
		/*
		if (thread!=null){
			thread.stop();
			thread = null;
		}
		*/
	}

	float ag = 0;
	
	//----------------------------------------------------------
	//メインループ
	//
	//----------------------------------------------------------
	public void MainLoop(){
		
		Matrix m = new Matrix();
		
		//射影変換マトリクス
		m.Projection((float)Math.PI/4,(float)SCREENH/(float)SCREENW);
		render.SetTransform(render.TRANSFORM_PROJ,m);

		//クリップマトリクス
		m.Clip(-1,1,2,2,100.f,4000.f);
		render.SetTransform(render.TRANSFORM_CLIP,m);

		//ビューポートマトリクス
		m.ViewPort(0,0,SCREENW,SCREENH);
		render.SetTransform(render.TRANSFORM_VIEWPORT,m);
		
		//カメラ１
		Vector v1 = new Vector((float)Math.sin(ag)*400, 100, (float)Math.cos(ag) * 600);
		Vector v2 = new Vector(0.f, 0.f, 0.f);
		Vector v3 = new Vector(0.f, 1.f, 0.f);
		m.View(v1, v2, v3);
		render.SetTransform(render.TRANSFORM_VIEW,m);

		//render.BeginScene();
		render.Clear();

		//オブジェクト描画
		m.Initialize();
		m.Scale(120.0f,120.0f,120.0f);
		m.RotateX(rx);
		m.RotateY(ry);
		m.RotateZ(rz);
		render.SetTransform(render.TRANSFORM_WORLD,m);
		render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);

		//バウンディングオブジェクトの描画
		render.DrawIndexedPrimitive(render.PRIMITIVE_3DLINE,bvertices,bindices);

		//render.EndScene(this,back);

		rx += .01;
		ry += .02;
		rz += .04;
		ag+=.04f;
	}
	
	/*
	//----------------------------------------------------------
	//フレームレート表示
	//
	//----------------------------------------------------------
	static float fps   = 0;
	static int   count = 59;
	static long  time  = System.currentTimeMillis();
	
	public void FrameRate(Graphics g){
		if (++count == 60){
			fps   = 1000 / (float)((System.currentTimeMillis() - time) / 60);
			time  = System.currentTimeMillis();
			count = 0;
		}
		g.setColor(Color.black);
		g.drawString("FPS " + fps,0,16);
		g.drawString("VTX " + vertices.length,0,32);
		
		if (flag) g.drawString("BOUNDING SPHERE",0,SCREENH-16);
		else      g.drawString("BOUNDING BOX   ",0,SCREENH-16);
	}
	*/

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
