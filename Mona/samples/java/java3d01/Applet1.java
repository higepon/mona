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
	*/
		
	static final int SCREENW     = 320;	//スクリーン高さ
	static final int SCREENH     = 200;	//スクリーン幅
	static final int FRAMERATE   = 10;	//フレームレート
	static final int DETAILLEVEL = 32;	//モデルの細かさ
	
	float rx;
	float ry;
	float rz;
	

	Vector    vertices[];	//頂点配列
	int[] pbuf;
	
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
		pbuf = new int[SCREENW * SCREENH];
		for (int i=0;i<SCREENW * SCREENH;i++){
				pbuf[i] = 0xffffffff;
		}
		
		//頂点データの作成
		SimpleShape ss = new SimpleShape();

		ss.CreateDonuts(DETAILLEVEL);
		
		//頂点リストを取得
		vertices = ss.GetVB();
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
		SimpleCanvas.create(pbuf, "VertexTransform", SCREENW, SCREENH);
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
		/*
		back.setColor(Color.white);
		back.fillRect(0,0,SCREENW,SCREENH);
		*/
		for (int i=0;i<SCREENW * SCREENH;i++){
				pbuf[i] = 0xffffffff;
		}

		Matrix m = new Matrix();			//マトリクス作成
		m.Initialize();						//単位行列で初期化
		m.Scale(80,80,80);					//拡大
		m.RotateX(rx);						//Ｘ軸回転
		m.RotateY(ry);						//Ｙ軸回転
		m.RotateZ(rz);						//Ｚ軸回転
		m.Translate(SCREENW/2,SCREENH/2,0);	//画面中央に平行移動

		//back.setColor(Color.black);
		for (int i=0;i<vertices.length;i++){

			//一次変換
			Vector v = m.Transform(vertices[i]);
	
			//点を打つ
			//back.fillRect((int)v.x,(int)v.y,1,1);
			pbuf[(int)v.x + (int)v.y * SCREENW] = 0;
		}

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
