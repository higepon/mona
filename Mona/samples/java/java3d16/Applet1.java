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
	MyTexture texture;
		
	static final int SCREENW     = 320;	//スクリーン高さ
	static final int SCREENH     = 160;	//スクリーン幅
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
		texture = new MyTexture();
		
		//テクスチャの設定
		render.SetTexture(texture);

		//モデルデータの作成
		SimpleShape ss = new SimpleShape();

//		ss.CreateTorus(6);
		ss.CreatePolygon(6);
		vertices = ss.GetVB();
		indices  = ss.GetIndices();		
		
		ss.CreateCylinder(6);
		vertices2 = ss.GetVB();
		indices2  = ss.GetIndices();		
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
		SimpleCanvas.create(render.pbuf,"FlatShadow",SCREENW,SCREENH);
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
		Matrix view = new Matrix();
		
		//射影変換マトリクス
		m.Projection(40,4000,(float)Math.PI/6,(float)SCREENH/(float)SCREENW);
		Render.SetTransform(render.TRANSFORM_PROJ,m);
				
		//レンダリング開始
		//Render.BeginScene();
		Render.SetViewPort(0,0,SCREENW,SCREENH);
		Render.Clear();

		//ビューマトリクス
		view.View(new Vector((float)Math.sin(ag) * 1000,(float)Math.sin(ag) * 400 + 600,(float)Math.cos(ag) * 600),new Vector(0,0,0),new Vector(0,1,0));
		
		//張り込んだオブジェクトを描画
		//カメラをXZ平面に対して反転
		view.m10 *= -1;
		view.m11 *= -1;
		view.m12 *= -1;
		Render.SetTransform(render.TRANSFORM_VIEW,view);		
		DrawObjects();

		//反転を元に戻す
		view.m10 *= -1;
		view.m11 *= -1;
		view.m12 *= -1;
		Render.SetTransform(render.TRANSFORM_VIEW,view);		
		Render.SetRenderState(Render.STATE_ALPHABLEND);

		//床描画
		m.Initialize();
		m.Scale(600,600,600);
		m.RotateX((float)Math.PI);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(render.PRIMITIVE_POLYGON,vertices,indices);

		//影描画
		Vector light = new Vector(.4f,.8f,0);
		light.Normalize();
		
		Matrix mShadow = new Matrix();
		mShadow.Shadow(light);
		mShadow.Mult(view);
		Render.SetRenderState(render.STATE_SHADOW);

		Matrix mm = new Matrix();
		
		float rate = 1.0f;
		for (int i=0;i<4;i++){
			mm.Initialize();
			mm.Scale(rate,rate,rate);
			mm.Mult(mShadow);
			Render.SetTransform(render.TRANSFORM_VIEW,mm);
			DrawObjects();
			rate -= .04f;
		}

		//オブジェクト描画
		Render.SetTransform(render.TRANSFORM_VIEW,view);
		Render.SetRenderState(0);
		DrawObjects();
		
		//レンダリング終了
		//Render.EndScene(this,back);
		
		ag -= .01;
		rx += .01;
		ry += .04;
	}
	
	public void DrawObjects()
	{
		Matrix m = new Matrix();
		m.Initialize();
		m.Scale(200,200,200);
		m.Translate(0,200,0);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(Render.PRIMITIVE_POLYGON,vertices2,indices2);
		
		m.Initialize();
		m.Scale(100,200,100);
		m.RotateX((float)Math.PI/2);
		m.Translate(0,200,250);
		m.RotateY(ry);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(Render.PRIMITIVE_POLYGON,vertices2,indices2);		

		m.Initialize();
		m.Scale(100,200,100);
		m.RotateX(-(float)Math.PI/2);
		m.Translate(0,200,-250);
		m.RotateY(ry);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(Render.PRIMITIVE_POLYGON,vertices2,indices2);		

		m.Initialize();
		m.Scale(100,200,100);
		m.RotateX(-(float)Math.PI/2);
		m.RotateY(-(float)Math.PI/2);
		m.Translate(250,200,0);
		m.RotateY(ry);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(Render.PRIMITIVE_POLYGON,vertices2,indices2);		

		m.Initialize();
		m.Scale(100,200,100);
		m.RotateX(-(float)Math.PI/2);
		m.RotateY((float)Math.PI/2);
		m.Translate(-250,200,0);
		m.RotateY(ry);
		Render.SetTransform(render.TRANSFORM_WORLD,m);
		Render.DrawIndexedPrimitive(Render.PRIMITIVE_POLYGON,vertices2,indices2);		
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
		g.setColor(Color.white);
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
