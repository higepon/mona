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

		//モデルデータの作成
		SimpleShape ss = new SimpleShape();

		ss.CreateBox();
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
		m.View(new Vector(0,0,-5),new Vector(0,0,0),new Vector(0,1,0));
		Render.SetTransform(render.TRANSFORM_VIEW,m);

		//レンダリング開始
		//Render.BeginScene();

		render.SetRenderState(0);
		render.SetViewPort(0,0,SCREENW/2,SCREENH);
		Render.Clear();
		DrawObjects();

		render.SetRenderState(render.STATE_DITHERING);
		render.SetViewPort(SCREENW/2,0,SCREENW/2,SCREENH);
		Render.Clear();
		DrawObjects();		

		//レンダリング終了
		//Render.EndScene(this,back);
		
		rx += .01f;
		ry += .02f;
		rz += .005f;
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
		m.RotateX(rx);
		m.RotateY(ry);
		m.RotateZ(rz);		
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
		g.drawString("16bit COLOR",8,SCREENH);
		g.drawString("16bit COLOR + DITHERING",SCREENW/2 + 8,SCREENH);
	}
	*/

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
