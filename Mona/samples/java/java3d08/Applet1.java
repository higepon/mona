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
	static final int SCREENH     = 136;	//スクリーン幅
	static final int FRAMERATE   = 10;	//フレームレート
	static final int DETAILLEVEL = 6;	//モデルの細かさ
	
	float rx;
	float ry;
	float rz;
	

	Vertex    vertices[];	//頂点配列
	int       indices [];	//インデックス配列

	Vertex    vertices2[];	//頂点配列
	int       indices2 [];	//インデックス配列

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

		//モデルデータの作成
		SimpleShape ss = new SimpleShape();

		ss.CreatePolygon(4);
		vertices2 = ss.GetVB();
		indices2  = ss.GetIndices();
		
		ss.CreateCylinder(DETAILLEVEL);
		vertices = ss.GetVB();
		indices  = ss.GetIndices();		
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
		m.Projection(40,4000,(float)Math.PI/6,(float)SCREENH/(float)SCREENW);
		Render.SetTransform(render.TRANSFORM_PROJ,m);
				
		//レンダリング開始
		//Render.BeginScene();

		//カメラ１
		m.View(new Vector((float)Math.sin(ag) * 600,200,(float)Math.cos(ag) * 1200),new Vector(0,0,0),new Vector(0,1,0));
		Render.SetTransform(render.TRANSFORM_VIEW,m);
		
		render.SetViewPort(0,0,SCREENW,SCREENH);
		Render.Clear();
		DrawObjects();

		//カメラ２
		m.View(new Vector((float)Math.sin(ag) * 600,(float)Math.sin(ag) * 200 + 400,(float)Math.cos(ag) * 600),new Vector(0,0,0),new Vector(0,1,0));
		Render.SetTransform(render.TRANSFORM_VIEW,m);

		render.SetViewPort(210,80,100,45);
		render.Clear();
		DrawObjects();

		//レンダリング終了
		//Render.EndScene(this,back);
		
		//枠描画
		//back.drawRect(210,80,100,45);
		for (int i = 0; i < 100; i++) {
			Render.pbuf[210 + i +  80 * SCREENW] = 0;
			Render.pbuf[210 + i + 124 * SCREENW] = 0;
		}
		for (int i = 0; i < 45; i++) {
			Render.pbuf[210 + (80 + i) * SCREENW] = 0;
			Render.pbuf[309 + (80 + i) * SCREENW] = 0;
		}
		
		ag -= .015;
	}

	//----------------------------------------------------------
	//オブジェクト描画
	//
	//----------------------------------------------------------
	void DrawObjects()
	{
		Matrix m = new Matrix();
		
		//円錐描画 * 8	
		float a = 0;
		for (int i=0;i<8;i++){
			m.Initialize();
			m.Scale(100,100,100);
			m.Translate((float)Math.cos(a) * 280,100,(float)Math.sin(a) * 280);
			Render.SetTransform(render.TRANSFORM_WORLD,m);
			Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);
			a += (float)Math.PI/8*2; 
		}
		m.Initialize();
		m.Scale(300,200,300);
		m.Translate(0,200,0);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);		
		
		//平面描画
		m.Initialize();
		m.Scale(400,400,400);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices2,indices2);
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
	}
	*/

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
