#if defined(OSASK)
#ifndef USE_CONSOLE
	#define printf(str, ...)
#else
	#define	CONS_IBUFSIZ	256
	#define	CONS_EBUFSIZ	 64

	struct CONSOLE {
		int x_size, y_size, x_cur, y_cur, color;
		int ibuf_rptr, ibuf_wptr, ebuf_rptr, ebuf_wptr, curflag;
		unsigned char *cons_buf, *input_buf, *echo_buf;
		struct LIB_TEXTBOX *textbox;
	};

	static struct LIB_WINDOW *console;
	static struct LIB_TEXTBOX *contitle;
	static struct CONSOLE *stdout;
	static char strbuf[256];

	static struct CONSOLE *copen(const int x_size, const int y_size,
		struct LIB_WINDOW *window, const int x0, const int y0, const int color, const int backcolor)
	/* コンソール初期化 */
	{
		struct CONSOLE *cons = (struct CONSOLE *) malloc(sizeof (struct CONSOLE));
		int i, j;
		char *p;
		cons->textbox = lib_opentextbox((backcolor >= 0) ? 0x0001 : 0x0000, 0,
			(backcolor >= 0) ? backcolor : 0, x_size, y_size, x0, y0, window, 0x00c0, 0);
		cons->x_size = x_size;
		cons->y_size = y_size;
		cons->color = color;
		cons->cons_buf  = (char *) malloc((x_size + 1) * (y_size + 1));
		cons->input_buf = (char *) malloc(CONS_IBUFSIZ);
		cons->echo_buf  = (char *) malloc(CONS_EBUFSIZ);
		cons->x_cur = 0;
		cons->y_cur = 0;
		cons->ibuf_rptr = 0;
		cons->ibuf_wptr = 0;
		cons->ebuf_rptr = 0;
		cons->ebuf_wptr = 0;
		cons->curflag = 0;
		p = cons->cons_buf;
		for (i = 0; i < y_size + 1; i++) {
			for (j = 0; j < x_size; j++)
				*p++ = ' ';
			*p++ = '\0';
		}
		return cons;
	}

	static void cons_sub_putcursor(struct CONSOLE *cons)
	/* カーソルを出す */
	{
		lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, "_");
		return;
	}

	static void cons_sub_erasecursor(struct CONSOLE *cons)
	/* カーソルを消す */
	{
		lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, " ");
		return;
	}

	static void cons_sub_scroll(struct CONSOLE *cons)
	/* スクロール */
	{
		char *s, *d;
		int i, j, xsiz1 = cons->x_size + 1;
		d = cons->cons_buf;
		s = d + cons->x_size + 1;
		for (i = 0; i < cons->y_size; i++) {
			for (j = 0; j < xsiz1; j++)
				*d++ = *s++;
			lib_putstring_ASCII(0x0000, 0, i, cons->textbox, cons->color, 0, d);
		}
		cons->y_cur--;
		return;
	}

	static void cputc(int c, struct CONSOLE *cons)
	/* コンソール一文字出力 */
	{
		if (cons->curflag)
			cons_sub_erasecursor(cons);
		if (c == '\n') {
			cons->x_cur = 0;
			if (++cons->y_cur == cons->y_size)
				cons_sub_scroll(cons);
		} else {
			char s[2];
			cons->cons_buf[cons->y_cur * (cons->x_size + 1) + cons->x_cur] = s[0] = (unsigned char) c;
			s[1] = '\0';
			lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, s);
			if (++cons->x_cur == cons->x_size) {
				cons->x_cur = 0;
				if (++cons->y_cur == cons->y_size)
					cons_sub_scroll(cons);
			}
		}
		if (cons->curflag)
			cons_sub_putcursor(cons);
		return;
	}

	static void cputs(const unsigned char *str, struct CONSOLE *cons)
	/* コンソールストリング出力 */
	{
		while (*str)
			cputc((int) *str++, cons);
		return;
	}

	static int printf(const char *format, ...)
	/* コンソール整形出力 */
	{
		int i;
		va_list ap;

		va_start(ap, format);
		i = vsprintf(strbuf, format, ap);
		va_end(ap);
		cputs(strbuf, stdout);
		return i;
	}
#endif
//USE_CONSOLE
#endif
// OSASK

#if defined(PEKOE)
	static char strbuf[256];

	static int printf(char *format, ...)
	/* コンソール整形出力 */
	{
		int i, I;
		va_list ap;

		va_start(ap, format);
		I = vsprintf(strbuf, format, ap);
		va_end(ap);
		for(i = 0; i < I; i++){
			putch(strbuf[i]);
		}
		return I;
	}
#endif
// PEKOE
