#include "jisx0201.h"
#include "jisx0208.h"

#if defined(OSASK)
/* static */ struct LIB_GRAPHBOX *gbox;

#if (!defined(LIB_GBOX_BUF))
	#define LIB_GBOX_BUF(gbox)   ((void *) ((char *) (gbox) + 64))
	#define LIB_GBOX_BUF8(gbox)  ((unsigned char *) LIB_GBOX_BUF(gbox))
	#define LIB_GBOX_BUF16(gbox) ((unsigned short *) LIB_GBOX_BUF(gbox))
	#define LIB_GBOX_BUF32(gbox) ((unsigned int *) LIB_GBOX_BUF(gbox))
#endif
#endif

/* 24bit -> 24bit */
#define getRGB24(r, g, b) ((r & 0xFFFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF))
/* 24bit -> 16bit */
#define getRGB16(r, g, b) ((r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3)

#define getAlpha(c) (0x100 - ((c >> 24) & 0xFF))
#define getRed(c)   ((c >> 16) & 0xFF)
#define getGreen(c) ((c >> 8) & 0xFF)
#define getBlue(c)  (c & 0xFF)

/* static */ uint32 bpp24_CalcAlpha(uint32 src, uint32 dst, uchar a) {
/* アルファ値を計算する */
	int16 r1, g1, b1, r2, g2, b2, r3, g3, b3;

	r1 = getRed(src)   + 1;
	g1 = getGreen(src) + 1;
	b1 = getBlue(src)  + 1;
	r2 = getRed(dst)   + 1;
	g2 = getGreen(dst) + 1;
	b2 = getBlue(dst)  + 1;
	//r3 = (r2 * a + r1 * (256 - a))/256 - 1;
	//g3 = (g2 * a + g1 * (256 - a))/256 - 1;
	//b3 = (b2 * a + b1 * (256 - a))/256 - 1;
	r3 = ((r2 * a) >> 8) + r1  - ((a * r1) >> 8) - 1;
	g3 = ((g2 * a) >> 8) + g1  - ((a * g1) >> 8) - 1;
	b3 = ((b2 * a) >> 8) + b1  - ((a * b1) >> 8) - 1;
	return getRGB24(r3 & 0xFF, g3, b3);
}

/* static */ void bpp24_UpdateRect(int x0, int y0, int x1, int y1){
/* 画面をフラッシュする */
	int x, y, w, h;

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= g_mainWinWidth)  x1 = g_mainWinWidth  - 1;
	if (y1 >= g_mainWinHeight) y1 = g_mainWinHeight - 1;
	x  = (x0 < x1) ? x0 : x1;
	y  = (y0 < y1) ? y0 : y1;
	w  = (x0 < x1) ? x1 - x0 + 1 : x0 - x1 + 1;
	h  = (y0 < y1) ? y1 - y0 + 1 : y0 - y1 + 1;

#if defined(OSASK)
	if (x0 == 0 && y0 == 0 && x1 == g_mainWinWidth - 1 && y1 == g_mainWinHeight - 1) {
		lib_flushgraphbox(0x8004, window, 0, 0, g_mainWinWidth, g_mainWinHeight, 0, LIB_GBOX_BUF32(gbox));
	}else{
		lib_flushgraphbox(0x8004, window, x, y, w, h, 
			(g_mainWinWidth - w) * 4, LIB_GBOX_BUF32(gbox) + y * g_mainWinWidth + x);
	}
#elif defined(MONACUI)
	if (x0 == 0 && y0 == 0 && x1 == g_mainWinWidth - 1 && y1 == g_mainWinHeight - 1) {
		screen->bitblt(screen, g_mainWinOffX, g_mainWinOffY, g_mainWinWidth, g_mainWinHeight, 
			vscreen, g_mainWinOffX, g_mainWinOffY);
	}else{
		screen->bitblt(screen, x + g_mainWinOffX, y + g_mainWinOffY, w, h, 
			vscreen, x + g_mainWinOffX, y + g_mainWinOffY);
	}
#elif defined(SDL)
	{
		SDL_Surface *bitmap;

		/* bitmapを作成 */
		// w, h, bpp, pit, rmask, gmask, bmask
		bitmap = SDL_CreateRGBSurfaceFrom((void *)image, g_mainWinWidth, g_mainWinHeight, 32, g_mainWinWidth * 4,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
		/* screenにbitmapを描画 */
		SDL_BlitSurface(bitmap, NULL, screen, NULL);
		if (x0 == 0 && y0 == 0 && x1 == g_mainWinWidth - 1 && y1 == g_mainWinHeight - 1) {
			/* screenの全領域を更新 */
			SDL_UpdateRect(screen, 0, 0, 0, 0);
		} else {
			/* screenの指定領域を更新 */
			SDL_UpdateRect(screen, x, y, w, h);
		}
	}
#elif defined(NWSOS)
#if 0
	if (x0 == 0 && y0 == 0 && x1 == g_mainWinWidth - 1 && y1 == g_mainWinHeight - 1) {
		/* screenの全領域を更新 */
		gfShowUpVirVRAM( NULL );
	} else {
		/* screenの指定領域を更新 */
		SZRECT rect;
		rect.m_tx = x;
		rect.m_ty = y;
		rect.m_sx = w;
		rect.m_sy = h;
		gfShowUpVirVRAM( &rect );
	}
#endif
	gfSetClientDC( g_hwndMain );
	gfPutImage( 0, 0, g_mainWinWidth, g_mainWinHeight, bitmap, IMAGETYPE_RGB888 );
#endif
}

/* static */ void bpp24_DrawPixcel(int x0, int y0, uint32 rgb24, int32 dop, int width, int height, uint32 *ibuff){
	// クリッピング
	if (x0 < g_mainWinCX0 || g_mainWinCX1 < x0 || 
		y0 < g_mainWinCY0 || g_mainWinCY1 < y0 || 
		x0 < 0 || g_mainWinWidth  <= x0 || 
		y0 < 0 || g_mainWinHeight <= y0) return;
	// スクリーンバッファ
	if (width == 0 && height == 0) {
		if (rgb24 >= 0xFF000000) {
			// 透過色（描画しない）
		}else{
			uint32 src = 0;
			if (0x1000000 <= rgb24 && rgb24 < 0xFF000000) {
				// アルファブレンディング
				src = image[y0 * g_mainWinWidth + x0];
				image[y0 * g_mainWinWidth + x0] = 
					bpp24_CalcAlpha(src, (uint32)rgb24, getAlpha(rgb24));
			} else if (dop == DRAW_AND) {
				image[y0 * g_mainWinWidth + x0] &= rgb24;
			} else if (dop == DRAW_OR) {
				image[y0 * g_mainWinWidth + x0] |= rgb24;
			} else if (dop == DRAW_XOR) {
				image[y0 * g_mainWinWidth + x0] ^= rgb24;
			} else {
				image[y0 * g_mainWinWidth + x0] = rgb24;
			}
			rgb24 = image[y0 * g_mainWinWidth + x0];
#if defined(OSASK)
			*(LIB_GBOX_BUF32(gbox) + y0 * g_mainWinWidth + x0) = rgb24;
#elif defined(MONA)
			bitmap->SetPixel(x0, y0, 
				System::Drawing::Color::FromArgb((rgb24 >>16) & 0xFF, (rgb24 >> 8) & 0xFF, rgb24 & 0xFF));
#elif defined(MONACUI)
			//screen.putPixel16(x0 + g_mainWinOffX, y0 + g_mainWinOffY, (0xffffff & rgb24));
			vscreen->putPixel16(x0 + g_mainWinOffX, y0 + g_mainWinOffY, (0xffffff & rgb24));
#elif defined(PEKOE)
			sys_gs_set_pixel_RGB(x0 + g_mainWinOffX, y0 + g_mainWinOffY, rgb24);
#elif defined(NWSOS)
			#if 0
			gfSetClientDC( g_hwndMain );
			gfSetDrawLayer( DRAW_LAYER_VIRVRAM );
			gfPushColorRGBPack(rgb24 & 0xffffff);
			gfPutPixel(x0 + g_mainWinOffX, y0 + g_mainWinOffY);
			gfDispopColor();
			#endif
			bitmap[(y0 * g_mainWinWidth + x0) * 3    ] = getBlue(rgb24);
			bitmap[(y0 * g_mainWinWidth + x0) * 3 + 1] = getGreen(rgb24);
			bitmap[(y0 * g_mainWinWidth + x0) * 3 + 2] = getRed(rgb24);
#endif
		}
	// イメージバッファ
	} else {
		if (dop == DRAW_AND) {
			ibuff[width * y0 + x0] &= rgb24;
		} else if (dop == DRAW_OR) {
			ibuff[width * y0 + x0] |= rgb24;
		} else if (dop == DRAW_XOR) {
			ibuff[width * y0 + x0] ^= rgb24;
		} else {
			ibuff[width * y0 + x0] = rgb24;
		}
	}
}

/* static */ void bpp24_DrawString(int x0, int y0, char* str, int32 fstyle, uint32 rgb24, int32 dop, int iw, int ih, uint32 *ibuff) {
/* グラフィックボックスに文字を描く */
	int i = 0, j = 0, k = 0, n = 0, n1 = 0, n2 = 0, cnt = 0, x1 = 0, y1 = 0;
	uchar c1 = 0, c2 = 0, c3 = 0;
	uint16 c = 0;

	//printf("%s\n", str);
	//while (str[i] != 0 && i < xstrlen(str)){
	for (i = 0; i < (int)xstrlen(str); i++) {
		// 文字列の終端
		if (str[i] == 0) break;
		// 1 バイト目
		c1 = (uchar)str[i];
		// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
		if (0x20 <= c1 && c1 <= 0x7F) {
			//printf("n = %d\n", c1);
			n = c1;
		// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
		} else if (0xC2 <= c1 && c1 <= 0xDF) {
			c2 = (uchar)str[++i];
			n = ((c1 & 0x1F) >> 6) | (c2 & 0x3F);
		// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
		} else if (0xE0 <= c1 && c1 <= 0xEF) {
			c2 = (uchar)str[++i];
			c3 = (uchar)str[++i];
			n = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
		} else {
			n = 0;
		}
		// 半角フォント描画
		if ((n1 = getHalfFontIndex(n)) != -1){
			for (j = 0; j < 12; j++){
				for (k = 7; k >= 2; k--){
					x1 = cnt * 6 + (7 - k) + x0;
					y1 = j + y0;
					c = jisx0201[n1 * 12 + j];
					if (((c >> k) & 0x1) == 1 && g_mainWinCX0 <= x1 && x1 < g_mainWinCX1 && y1 < g_mainWinCY1)
					//if (((c >> k) & 0x1) == 1)
					{
						bpp24_DrawPixcel(x1, y1, rgb24, dop, iw, ih, ibuff);
						// 太字
						if (fstyle == Font_BOLD && x1 + 1 < g_mainWinCX1) {
						//if (fstyle == Font_BOLD) {
							bpp24_DrawPixcel(x1 + 1, y1, rgb24, dop, iw, ih, ibuff);
						}
					}
				}
			}
			cnt++;
		// 全角フォント描画
		} else if ((n2 = getFullFontIndex(n)) != -1){
			if(jisx0208 != NULL){
				for (j = 0; j < 12; j++){
					for (k = 15; k >= 4; k--){
						x1 = cnt * 6 + (15 - k) + x0;
						y1 = j + y0;
						c = (jisx0208[n2 * 24 + 2 * j] << 8) | jisx0208[n2 * 24 + 2 * j + 1];
						//if (((c >> k) & 0x1) == 1 && g_mainWinCX0 <= x1 && x1 < g_mainWinCX1 && y1 < g_mainWinCY1)
						if (((c >> k) & 0x1) == 1)
						{
							bpp24_DrawPixcel(x1, y1, rgb24, dop, iw, ih, ibuff);
							// 太字
							// 太字
							//if (fstyle == Font_BOLD && x1 + 1 < g_mainWinCX1) {
							if (fstyle == Font_BOLD) {
								bpp24_DrawPixcel(x1 + 1, y1, rgb24, dop, iw, ih, ibuff);
							}
						}
					}
				}
			}
			cnt += 2;
		}
	}
	if (iw == 0 && ih == 0) {
		//bpp24_UpdateRect(0, 0, g_mainWinWidth, g_mainWinHeight);
		bpp24_UpdateRect(x0, y0, x1, y0 + 12);
	}
}

/* static */ void bpp24_FillRect(int x0, int y0, int w, int h, uint32 rgb24, int32 dop, int iw, int ih, uint32 *ibuff) {
/* 矩形を塗りつぶす */
	int i, j;

	// クリッピング
	if (x0 < g_mainWinCX0 || g_mainWinCX1 < x0 || 
		y0 < g_mainWinCY0 || g_mainWinCY1 < y0 || 
		x0 < 0 || g_mainWinWidth  <= x0 || 
		y0 < 0 || g_mainWinHeight <= y0) return;

	for (i = x0; i < x0 + w; i++) {
		for (j = y0; j < y0 + h; j++) {
			bpp24_DrawPixcel(i, j, rgb24, dop, iw, ih, ibuff);
		}
	}

	if (iw == 0 && ih == 0)
		//bpp24_UpdateRect(0, 0, g_mainWinWidth, g_mainWinHeight);
		bpp24_UpdateRect(x0, y0, x0 + w, y0 + h);
}

/* static */ void bpp24_DrawLine(int x0, int y0, int x1, int y1, uint32 rgb24, int32 dop, int iw, int ih, uint32* ibuff) {
/* グラフィックボックスに直線を引く */
	int E,x,y,dx,dy,sx,sy,i;

	// クリッピング
	if (x0 < g_mainWinCX0 || g_mainWinCX1 < x0 || 
		y0 < g_mainWinCY0 || g_mainWinCY1 < y0 || 
		x0 < 0 || g_mainWinWidth  <= x0 || 
		y0 < 0 || g_mainWinHeight <= y0) return;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* 傾きが1以下の場合 */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			bpp24_DrawPixcel(x, y, rgb24, dop, iw, ih, ibuff);
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
	/* 傾きが1より大きい場合 */
	} else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			bpp24_DrawPixcel(x, y, rgb24, dop, iw, ih, ibuff);
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}

	if (iw == 0 && ih == 0)
		//bpp24_UpdateRect(0, 0, g_mainWinWidth, g_mainWinHeight);
		bpp24_UpdateRect(x0, y0, x1, y1);
}

/* static */ int bpp24_DecodeBmp(uchar *p, uint32 *w, uint32 *h, uint32 **list){
	uint32 bitmapOffset, infoSize, width, height, bpp, compression, colors, scanlen;
	uint32 palette256[256];
	uchar *ip, c;
	int32 x, y, i;

	// header (54 bytes)
	// 0-1   magic chars 'BM'
	// 2-5   uint32 filesize (not reliable)
	// 6-7   uint16 0
	// 8-9   uint16 0
	// 10-13 uint32 bitmapOffset
	// 14-17 uint32 info size
	// 18-21 int32  width
	// 22-25 int32  height
	// 26-27 uint16 nplanes
	// 28-29 uint16 bits per pixel
	// 30-33 uint32 compression flag
	// 34-37 uint32 image size in bytes
	// 38-41 int32  biXPelsPerMeter
	// 32-45 int32  biYPelsPerMeter
	// 46-49 uint32 colors used
	// 50-53 uint32 important color count

	if (p[0] != 'B' || p[1] != 'M')
		return 0; // not a BMP file
	bitmapOffset = inGetUInt32(&p[10]);
	infoSize = inGetUInt32(&p[14]);
	if (infoSize != 40)
		return 0; // old-style BMP
	width = inGetUInt32(&p[18]);
	height = inGetUInt32(&p[22]);
	if (width > 65535 || height > 65535)
		return 0; // bad width/height
	bpp = inGetUInt16(&p[28]);
	if (bpp != 1 && bpp != 4 && bpp != 8)
		return 0; // not a 2, 16 or 256 color image
	compression = inGetUInt32(&p[30]);
	if (compression != 0)
		return 0; // compressed image
	colors = inGetUInt32(&p[46]);
	// default system color
	palette256[0]  = 0x000000; palette256[1]  = 0x800000; palette256[2]  = 0x008000; palette256[3]  = 0x808000;
	palette256[4]  = 0x000080; palette256[5]  = 0x800080; palette256[6]  = 0x008080; palette256[7]  = 0xC6C6C6;
	palette256[8]  = 0x808080; palette256[9]  = 0xff0000; palette256[10] = 0x00ff00; palette256[11] = 0xffff00;
	palette256[12] = 0x0000ff; palette256[13] = 0xff00ff; palette256[14] = 0x00ffff; palette256[15] = 0xffffff;
	// initialize color palette
	for (i = 0; i < (int)colors; i++){
		palette256[i] = inGetUInt32(&p[54 + i * 4]);
		//printf("palette %d = %d\n", i, palette256[i]);
	}

	*w = width;
	*h = height;
	*list = (uint32 *)xmalloc(sizeof(uint32) * width * height);

	//scanlen = (width * bpp + 7) / 8; // # bytes
	//scanlen = ((scanlen + 3) / 4) * 4; // end on 32 bit boundry
	scanlen = (int)((width * bpp + 31) / 32) * 4;
	ip = &p[bitmapOffset];
	//printf("%d %d %d %d %d\n", bpp, colors, width, height, scanlen);
	for (y = 0; y < (int)height; y++)
	{
		if(bpp == 1){
			for (x = 0; x < (int)(width/8); x++){
				//c = p[bitmapOffset +  y * width + x];
				c = *ip;
				for (i = 7; i >= 0; i--){
					if (((c >> i) & 1) == 1){
						//bpp24_DrawPixcel(x * 8 + (7 - i), height - y, 0xffffff, DRAW_OVER, 0);
						bpp24_DrawPixcel(x * 8 + (7 - i), height - y - 1, 0xffffff, DRAW_OVER, width, height, *list);
					}else{
						//bpp24_DrawPixcel(x * 8 + (7 - i), height - y, 0x000000, DRAW_OVER, 0);
						bpp24_DrawPixcel(x * 8 + (7 - i), height - y - 1, 0x000000, DRAW_OVER, width, height, *list);
					}
				}
				ip++;
			}
			//for (x = 0; x < scanlen; x++)
			//	ip++;
			ip += (scanlen - width/8);
		}else if(bpp == 4){
			for (x = 0; x < (int)(width/2); x++){
				//c = p[bitmapOffset +  y * width + x];
				c = *ip;
				//bpp24_DrawPixcel(x * 2, height - y, palette256[(c >> 4) & 0xf], DRAW_OVER, 0);
				//bpp24_DrawPixcel(x * 2 + 1, height - y, palette256[c & 0xf], DRAW_OVER, 0);
				bpp24_DrawPixcel(x * 2, height - y - 1, palette256[(c >> 4) & 0xf], DRAW_OVER, width, height, *list);
				bpp24_DrawPixcel(x * 2 + 1, height - y - 1, palette256[c & 0xf], DRAW_OVER, width, height, *list);
				ip++;
			}
			//for (x = 0; x < scanlen - width/2; x++)
			//	ip++;
			ip += (scanlen - width/2);
		}else if(bpp == 8){
			for (x = 0; x < (int)width; x++){
				//c = p[bitmapOffset +  y * width + x];
				c = *ip;
				//bpp24_DrawPixcel(x, height - y, palette256[c & 0xff], DRAW_OVER, 0);
				bpp24_DrawPixcel(x, height - y - 1, palette256[c & 0xff], DRAW_OVER, width, height, *list);
				ip++;
			}
			//for (x = 0; x < scanlen - width; x++)
			//	ip++;
			ip += (scanlen - width);
		}
	}

	return 1;
}
