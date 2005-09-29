/*
import java.awt.*;
import java.applet.*;
import java.awt.image.*;
*/

public class Applet1 //extends Applet implements Runnable
{
	/*
	Thread   thread;	//スレッド
	Graphics back;		//バックバッファ
	Image    offimage;
	*/
		
	static final int FRAMERATE   = 20;	//フレームレート
			
	Render    render = null;
	
	static int SCREENW;
	static int SCREENH;
	
	PRIMITIVE obj;
	
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
		SCREENW = 320; //this.getSize().width;
		SCREENH = 200; //this.getSize().height;
		
		render = new Render(SCREENW,SCREENH);
		Render.clear();
				
		//射影変換マトリクス
		Matrix proj = new Matrix();
		Matrix m    = new Matrix();
		proj.Initialize();
		m.Projection((float)Math.PI/6,(float)SCREENH/(float)SCREENW);
		proj.Mult(m);
		m.Clip(-1,1,2,2,100.f,4000.f);
		proj.Mult(m);
		m.ViewPort(0,0,SCREENW,SCREENH);
		proj.Mult(m);
		render.setTransform(render.TRANSFORM_PROJ,proj);
	}

	/*
	public void start()
	{
		thread = new Thread(this);
		if (thread!=null) thread.start();
	}
	
	public void paint(Graphics g)
	{
		g.drawImage(offimage,0,0,null);
	}
	
	public void update(Graphics g)
	{	
		mainLoop ();
		paint    (g);
		frameRate(g);
	}
	*/

	public void run(){
		
		//シーン初期化
		/*
		Graphics g = getGraphics();
		g.setColor(Color.white);
		g.fillRect(0,0,SCREENW,SCREENH);
		*/
		render.clear();
		
		//オブジェクト作成
//		obj = SimpleShape.createSphere(16);
		obj = SimpleShape.createDonuts(24);
		obj.texture = loadTexture("envmap.gif");
				
		/*
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
	float rx,ry,rz;
	
	//----------------------------------------------------------
	//メインループ
	//
	//----------------------------------------------------------
	public void MainLoop(){
		
		Matrix m = new Matrix();
		
		//レンダリング開始
		//render.beginScene();
				
		//バッファをクリア
		render.clear();
		
		//ライト設定
		render.setLight(new Vector(0,0,1));

		//ビューマトリクス設定
		m.View(new Vector((float)Math.sin(ag) * 800,300,(float)Math.cos(ag) * 800),new Vector(0,0,0),new Vector(0,1,0));
		render.setTransform(render.TRANSFORM_VIEW,m);		

		//オブジェクト描画
		m.Initialize();
		m.Scale(180,180,180);
		m.RotateX(rx);
		m.RotateY(ry);
		m.RotateZ(rz);
		render.setTransform(render.TRANSFORM_WORLD ,m);
		render.drawPrimitive(obj);
		
		//レンダリング終了
		//offimage = render.endScene(this);
		
		rx += .01;
		ry += .001;
		rz += .005;
	}
	
	/*
	//----------------------------------------------------------
	//フレームレート表示
	//
	//----------------------------------------------------------
	static float fps   = 0;
	static int   count = 59;
	static long  time  = System.currentTimeMillis();
	
	public void frameRate(Graphics g){
		if (++count == 60){
			fps   = 1000 / (float)((System.currentTimeMillis() - time) / 60);
			time  = System.currentTimeMillis();
			count = 0;
		}
		g.setColor(Color.black);
		g.drawString("FPS " + fps,0,16);
		g.drawString("VTX " + obj.vertex.length,0,32);
	}
	*/
	
	//----------------------------------------------------------
	//テクスチャのロード
	//
	//----------------------------------------------------------
	public TEXTURE loadTexture(String fname)
	{
		if (fname == null) return null;
		
		//テクスチャの新規作成
		TEXTURE tex = new TEXTURE();

		/*
		//テクスチャファイルのロード
		Image img = getImage(getCodeBase(),fname);
										 		
		//ロードが完了するまで待つ
		MediaTracker tracker = new MediaTracker(this);
		tracker.addImage(img,0);
		try{
			tracker.waitForAll();
		}catch(Exception e){
			return null;
		}
		*/
		MyTexture img = new MyTexture();

		//テクスチャ情報設定
		tex.name = fname;
		
		tex.w = img.width;  //getWidth (null);
		tex.h = img.height; //getHeight(null);
		
		tex.shift = 0;
		for (int i=0;i<32;i++)
			if ((((tex.w-1)>>i)&1) > 0)
				tex.shift++;
		
		//ピクセル情報取得
		/*
		tex.img = new int[tex.w * tex.h];
		
		PixelGrabber pg = new PixelGrabber(img,0,0,tex.w,tex.h,tex.img,0,tex.w);
		
		try{
			pg.grabPixels();
		}catch(InterruptedException e){
			return null;
		}
		*/
		tex.img = img.data;

		return tex;
	}

	public static void main(String[] args){
		Applet1 applet1 = new Applet1();
		applet1.init();
		applet1.run();
		applet1.stop();
	}
}
