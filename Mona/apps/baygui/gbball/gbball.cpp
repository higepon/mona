#include <baygui.h>

static int color4bit[16][3] = {
	{   0,   0,   0 }, { 128,  0,   0 }, {  0, 128,   0 }, { 128, 128,   0},
	{   0,   0, 128 }, { 128,  0, 128 }, {  0, 128, 128 }, { 192, 192, 192},
	{ 128, 128, 128 }, { 255,  0,   0 }, {  0, 255,   0 }, { 255, 255,   0},
	{   0,   0, 255 }, { 255,  0, 255 }, {  0, 255, 255 }, { 255, 255, 255}
};

static int point[16][2] = {
	{ 196, 100 }, { 187,  61 }, { 164,  29 }, { 129,   9 }, {  90,   5 },
	{  53,  17 }, {  23,  44 }, {   7,  81 }, {   7, 119 }, {  23, 156 },
	{  53, 183 }, {  90, 195 }, { 129, 191 }, { 164, 171 }, { 187, 139 },
	{ 196, 100 }
};

class GBBall : public Window {
private:
	Label *label;

public:
	GBBall(){
		setRect((800 - 212) / 2, (600 - 228) / 2, 212, 228);
		setTitle("bball");
	}
	void onPaint(Graphics *g) {
		int i, j;
		g->setColor(0, 0, 0);
		g->fillRect(0, 0, 200, 200);
		for (i = 0; i <= 14; i++) {
			int x0, y0, dis;
			x0 = point[i][0];
			y0 = point[i][1];
			for (j = i + 1; j <= 15; j++) {
				dis = j - i; /* 2‚Â‚Ì“_‚Ì‹——£ */
				if (dis >= 8)
				dis = 15 - dis; /* ‹t‰ñ‚è‚É”‚¦‚é */
				if (dis != 0) {
				g->setColor(color4bit[16 - dis][0],color4bit[16 - dis][1],color4bit[16 - dis][2]);
				g->drawLine(x0, y0, point[j][0], point[j][1]);
				}
			}
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GBBall *bball = new GBBall();
	bball->run();
	delete(bball);
	return 0;
}
