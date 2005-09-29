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
	Texture2 texture;
		
	static final int SCREENW     = 320;	//スクリーン高さ
	static final int SCREENH     = 160;	//スクリーン幅
	static final int FRAMERATE   = 10;	//フレームレート
	static final int DETAILLEVEL = 12;	//モデルの細かさ
	
	float rx;
	float ry;
	float rz;
	

	Vertex    vertices[];	//頂点配列
	int       indices [];	//インデックス配列
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
		texture = new Texture2();
		
		//テクスチャの設定
		render.SetTexture(texture);

		//モデルデータの作成
		SimpleShape ss = new SimpleShape();

		ss.CreatePolygon(8);
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
		m.Projection(0,4,(float)Math.PI/4,(float)SCREENH/(float)(SCREENW/2));
		Render.SetTransform(render.TRANSFORM_PROJ,m);
				
		//レンダリング開始
		//Render.BeginScene();

		//カメラ
		m.View(new Vector((float)Math.sin(ag) * 2,(float)Math.sin(ag) * 2 + 3,(float)Math.cos(ag) * 4),new Vector(0,0,0),new Vector(0,1,0));
		Render.SetTransform(render.TRANSFORM_VIEW,m);
		
		render.SetRenderState(0);
		render.SetViewPort(0,0,SCREENW/2,SCREENH);
		Render.Clear();
		DrawObjects();

		render.SetRenderState(render.STATE_BILINER);
		render.SetViewPort(SCREENW/2,0,SCREENW/2,SCREENH);
		Render.Clear();
		DrawObjects();
		
		//レンダリング終了
		//Render.EndScene(this,back);
		
		ag -= .015;
	}

	//----------------------------------------------------------
	//オブジェクト描画
	//
	//----------------------------------------------------------
	void DrawObjects()
	{
		Matrix m = new Matrix();

		//平面描画
		m.Initialize();
		m.Scale(2,2,2);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);
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
		g.drawString("NO FILTER",8,SCREENH);
		g.drawString("BI-LINER FILTER",164,SCREENH);
	}
	*/

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
