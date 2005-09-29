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
	static final int DETAILLEVEL = 16;	//モデルの細かさ
	
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
		texture = new Texture();
		
		//テクスチャの設定
		render.SetTexture(texture);

		//頂点データの作成
		SimpleShape ss = new SimpleShape();

		ss.CreateDonuts(DETAILLEVEL);
		
		//頂点リストを取得
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

	//----------------------------------------------------------
	//メインループ
	//
	//----------------------------------------------------------
	public void MainLoop(){

		//画面クリア
		Render.Clear();
		
		Matrix m = new Matrix();			//マトリクス作成
		m.Initialize();						//単位行列で初期化
		m.Scale(80,80,80);					//拡大
		m.RotateX(rx);						//Ｘ軸回転
		m.RotateY(ry);						//Ｙ軸回転
		m.RotateZ(rz);						//Ｚ軸回転
		m.Translate(SCREENW/2,SCREENH/2,200);

		Render.SetTransform(m);
		
		//Render.BeginScene();
		
		Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);
		//Render.EndScene(this,back);

		//回転角更新
		rx += 0.01;
		ry += 0.02;
		rz += 0.005;
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
	}
	*/

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
