package org.monaos;

public class Keys {
	public static final int KEY_MODIFIER_DOWN  = 0x01;
	public static final int KEY_MODIFIER_UP    = 0x02;
	public static final int KEY_MODIFIER_SHIFT = 0x04;
	public static final int KEY_MODIFIER_ALT   = 0x08;
	public static final int KEY_MODIFIER_CTRL  = 0x10;
	public static final int KEY_MODIFIER_WIN   = 0x20;
	public static final int KEY_MODIFIER_MENU  = 0x40;
	public static final int KEY_MODIFIER_CHAR  = 0x80;
	
	public static final int US102            = 0;
	public static final int JP109            = 1;
	
	public static final int Back             = 8;
	public static final int Tab              = 9;
	public static final int Enter            = 13;
	public static final int ShiftKey         = 16;
	public static final int ControlKey       = 17;
	public static final int Menu             = 18;
	public static final int Pause            = 19;
	public static final int CapsLock         = 20;
	public static final int KanaMode         = 21;
	public static final int KanjiMode        = 25;
	public static final int Escape           = 27;
	public static final int IMEConvert       = 28;
	public static final int IMENonconvert    = 29;
	public static final int IMEAceept        = 30;
	public static final int IMEModeChange    = 31;
	public static final int Space            = 32;
	public static final int PageUp           = 33;
	public static final int PageDown         = 34;
	public static final int End              = 35;
	public static final int Home             = 36;
	public static final int Left             = 37;
	public static final int Up               = 38;
	public static final int Right            = 39;
	public static final int Down             = 40;
	public static final int PrintScreen      = 44;
	public static final int Insert           = 45;
	public static final int Delete           = 46;
	public static final int D0               = 48;
	public static final int D1               = 49;
	public static final int D2               = 50;
	public static final int D3               = 51;
	public static final int D4               = 52;
	public static final int D5               = 53;
	public static final int D6               = 54;
	public static final int D7               = 55;
	public static final int D8               = 56;
	public static final int D9               = 57;
	public static final int A                = 65;
	public static final int B                = 66;
	public static final int C                = 67;
	public static final int D                = 68;
	public static final int E                = 69;
	public static final int F                = 70;
	public static final int G                = 71;
	public static final int H                = 72;
	public static final int I                = 73;
	public static final int J                = 74;
	public static final int K                = 75;
	public static final int L                = 76;
	public static final int M                = 77;
	public static final int N                = 78;
	public static final int O                = 79;
	public static final int P                = 80;
	public static final int Q                = 81;
	public static final int R                = 82;
	public static final int S                = 83;
	public static final int T                = 84;
	public static final int U                = 85;
	public static final int V                = 86;
	public static final int W                = 87;
	public static final int X                = 88;
	public static final int Y                = 89;
	public static final int Z                = 90;
	public static final int LWin             = 91;
	public static final int RWin             = 92;
	public static final int Apps             = 93;
	public static final int NumPad0          = 96;
	public static final int NumPad1          = 97;
	public static final int NumPad2          = 98;
	public static final int NumPad3          = 99;
	public static final int NumPad4          = 100;
	public static final int NumPad5          = 101;
	public static final int NumPad6          = 102;
	public static final int NumPad7          = 103;
	public static final int NumPad8          = 104;
	public static final int NumPad9          = 105;
	public static final int Multiply         = 106;
	public static final int Add              = 107;
	public static final int Separator        = 108;
	public static final int Subtract         = 109;
	public static final int Decimal          = 110;
	public static final int Divide           = 111;
	public static final int F1               = 112;
	public static final int F2               = 113;
	public static final int F3               = 114;
	public static final int F4               = 115;
	public static final int F5               = 116;
	public static final int F6               = 117;
	public static final int F7               = 118;
	public static final int F8               = 119;
	public static final int F9               = 120;
	public static final int F10              = 121;
	public static final int F11              = 122;
	public static final int F12              = 123;
	public static final int NumLock          = 144;
	public static final int Scroll           = 145;
	public static final int LShiftKey        = 160;
	public static final int RShiftKey        = 161;
	public static final int LControlKey      = 162;
	public static final int RControlKey      = 163;
	public static final int LMenu            = 164;
	public static final int RMenu            = 165;
	public static final int OemSemicolon     = 186;
	public static final int Oemplus          = 187;
	public static final int Oemcomma         = 188;
	public static final int OemMinus         = 189;
	public static final int OemPeriod        = 190;
	public static final int OemQuestion      = 191;
	public static final int Oemtilde         = 192;
	public static final int OemOpenBrackets  = 219;
	public static final int OemPipe          = 220;
	public static final int OemCloseBrackets = 221;
	public static final int OemQuotes        = 222;
	public static final int OemBackslash     = 226;

	private int mapUS102[] = {
		0        , Escape  , D1             , D2              , D3       , D4         , D5       , D6          ,
		D7       , D8      , D9             , D0              , OemMinus , Oemplus    , Back     , Tab         ,
		Q        , W       , E              , R               , T        , Y          , U        , I           ,
		O        , P       , OemOpenBrackets, OemCloseBrackets, Enter    , LControlKey, A        , S           ,
		D        , F       , G              , H               , J        , K          , L        , OemSemicolon,
		OemQuotes, Oemtilde, LShiftKey      , OemPipe         , Z        , X          , C        , V           ,
		B        , N       , M              , Oemcomma        , OemPeriod, OemQuestion, RShiftKey, Multiply    ,
		LMenu    , Space   , CapsLock       , F1              , F2       , F3         , F4       , F5          ,
		F6       , F7      , F8             , F9              , F10      , NumLock    , Scroll   , NumPad7     ,
		NumPad8  , NumPad9 , Subtract       , NumPad4         , NumPad5  , NumPad6    , Add      , NumPad1     ,
		NumPad2  , NumPad3 , NumPad0        , Decimal         , 0        , 0          , 0        , F11         ,
		F12      , 0       , 0              , 0               , 0        , 0          , 0        , 0           ,
	};

	private int mapUS102E0[] = {
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , Separator, RControlKey, 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , Divide     , 0, PrintScreen,
		RMenu, 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, Home       ,
		Up   , PageUp  , 0     , Left  , 0        , Right      , 0, End        ,
		Down , PageDown, Insert, Delete, 0        , 0          , 0, 0          ,
		0    , 0       , 0     , LWin  , RWin     , Apps       , 0, 0          ,
	};

	private int mapJP109[] = {
		0           , Escape    , D1       , D2              , D3       , D4         , D5       , D6      ,
		D7          , D8        , D9       , D0              , OemMinus , OemQuotes  , Back     , Tab     ,
		Q           , W         , E        , R               , T        , Y          , U        , I       ,
		O           , P         , Oemtilde , OemOpenBrackets , Enter    , LControlKey, A        , S       ,
		D           , F         , G        , H               , J        , K          , L        , Oemplus ,
		OemSemicolon, KanjiMode , LShiftKey, OemCloseBrackets, Z        , X          , C        , V       ,
		B           , N         , M        , Oemcomma        , OemPeriod, OemQuestion, RShiftKey, Multiply,
		LMenu       , Space     , CapsLock , F1              , F2       , F3         , F4       , F5      ,
		F6          , F7        , F8       , F9              , F10      , NumLock    , Scroll   , NumPad7 ,
		NumPad8     , NumPad9   , Subtract , NumPad4         , NumPad5  , NumPad6    , Add      , NumPad1 ,
		NumPad2     , NumPad3   , NumPad0  , Decimal         , 0        , 0          , 0        , F11     ,
		F12         , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
		0           , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
		0           , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
		KanaMode    , 0         , 0        , OemBackslash    , 0        , 0          , 0        , 0       ,
		0           , IMEConvert, 0        , IMENonconvert   , 0        , OemPipe    , 0        , 0       ,
	};

	private int mapJP109E0[] = {
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , Separator, RControlKey, 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , Divide     , 0, PrintScreen,
		RMenu, 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, Home       ,
		Up   , PageUp  , 0     , Left  , 0        , Right      , 0, End        ,
		Down , PageDown, Insert, Delete, 0        , 0          , 0, 0          ,
		0    , 0       , 0     , LWin  , RWin     , Apps       , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
		0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
	};

	private char keyTopMapUS102[] = {
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ';', '+', ',', '-', '.', '/',
		'~',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '[','\\', ']','\'',  0 ,
		 0 ,  0 ,'\\',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	};

	private char keyTopMapUS102S[] = {
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		')', '!', '@', '#', '$', '%', '^', '&', '*', '(',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ':', '=', '<', '_', '>', '?',
		'`',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '{', '|', '}', '"',  0 ,
		 0 ,  0 , '_',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	};

	private char keyTopMapJP109[] = {
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ':', ';', ',', '-', '.', '/',
		'@',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '[','\\', ']', '^',  0 ,
		 0 ,  0 ,'\\',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	};

	private char keyTopMapJP109S[] = {
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 , '!', '"', '#', '$', '%', '&','\'', '(', ')',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0 ,  0 ,  0 ,  0 ,  0 ,
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '*', '+', '<', '=', '>', '?',
		'`',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '{', '|', '}', '~',  0 ,
		 0 ,  0 , '_',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	};

	public char ToChar(KeyInfo keyInfo) {

		char result = '\0';

		if ((keyInfo.modifiers & KEY_MODIFIER_SHIFT) != 0) {
			result = keyTopMapJP109S[keyInfo.keycode & 0xff];
		} else {
			result = keyTopMapJP109[keyInfo.keycode & 0xff];
		}

		return result;
	}

	public boolean IsToChar(KeyInfo keyInfo) {

		char code = '\0';

		if ((keyInfo.modifiers & KEY_MODIFIER_SHIFT) != 0) {
			code = keyTopMapJP109S[keyInfo.keycode & 0xff];
		} else {
			code = keyTopMapJP109[keyInfo.keycode & 0xff];
		}

		if(code != '\0') return true;

		return false;
	}

	public char ToChar(KeyInfo keyInfo, char map[], char maps[]) {

		char result = '\0';

		if ((keyInfo.modifiers & KEY_MODIFIER_SHIFT) != 0) {
			result = maps[keyInfo.keycode & 0xff];
		} else {
			result = map[keyInfo.keycode & 0xff];
		}

		return result;
	}

	public boolean IsToChar(KeyInfo keyInfo, char map[], char maps[]) {

		char code = '\0';

		if ((keyInfo.modifiers & KEY_MODIFIER_SHIFT) != 0) {
			code = maps[keyInfo.keycode & 0xff];
		} else {
			code = map[keyInfo.keycode & 0xff];
		}

		if(code != '\0') return true;

		return false;
	}

	public int[] GetKeyMap(int kbdt) {
		switch (kbdt) {
			case US102:
				return mapUS102;
			case JP109:
				return mapJP109;
		}
		return null;
	}

	public int[] GetKeyMapE0(int kbdt) {
		switch (kbdt) {
			case US102:
				return mapUS102E0;
			case JP109:
				return mapJP109E0;
		}
		return null;
	}

	public char[] GetKeyTopMap(int kbdt) {
		switch (kbdt) {
			case US102:
				return keyTopMapUS102;
			case JP109:
				return keyTopMapJP109;
		}
		return null;
	}

	public char[] GetKeyTopMapS(int kbdt) {
		switch (kbdt) {
			case US102:
				return keyTopMapUS102S;
			case JP109:
				return keyTopMapJP109S;
		}
		return null;
	}
}
