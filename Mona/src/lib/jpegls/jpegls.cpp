/*
 * JPEG decoding engine for DCT-baseline
 *
 *      copyrights 2003 by nikq | nikq::club.
 */


#include <jpegls.h>


// コンストラクタ
CJPEGLS::CJPEGLS()
{
    init();
    decode_init();
}
CJPEGLS::~CJPEGLS()
{
    //特になし
}

int CJPEGLS::Open( unsigned char *data,int size )
{
    this->data= data;
    data_size = size;
    data_index= 0;

    //ヘッダのパース
    return header();
}

void CJPEGLS::GetInfo( int* _width, int* _height )
{
    *_width  = width;
    *_height = height;
}

void CJPEGLS::Decode( unsigned char * dest )
{
    decode(dest);
}



// -------------------------- I/O ----------------------------

// ビッグエンディアンっぽい
unsigned char CJPEGLS::get_byte(void)
{
    unsigned char c;

    // EOF
    if(data_index >= data_size)
        return 0;

    c = data[ data_index++ ];
    return c;
}
unsigned short CJPEGLS::get_word(void)
{
    unsigned char h,l;

    h = get_byte();
    l = get_byte();

    return (h<<8)|l;
}

unsigned short CJPEGLS::get_bits(int bit)
{
    unsigned char  c;
    unsigned short ret;

    while( bit_remain <= 16 ){
        c = get_byte();
        if(c == 0xFF)
            get_byte();

        bit_buff = (bit_buff << 8) | c;
        bit_remain += 8;
    }

    ret = (bit_buff>>(bit_remain - bit))&((1<<bit)-1);
    bit_remain -= bit;

    return ret;
}

// ------------------------ JPEG セグメント実装 -----------------

// 未対応なのはすっ飛ばす
void CJPEGLS::skip(void)
{
    unsigned w;

    w = get_word() - 2;
    data_index += w;
}

// start of frame
void CJPEGLS::sof(void)
{
    unsigned char c,n;
    int i,h,v;

    //printf("--- SOF ---\n");

    c = get_word();
    c = get_byte(); // bpp

    height = get_word();
    width  = get_word();

    n = get_byte();    // Num of compo
    compo_count = n; // nf

    for(i=0;i<n;i++) {
        compo_id[i] = get_byte();

        c = get_byte();
        //printf("%02x\n",c);
        compo_sample[i] = c;
        h = (c>>4) & 0x0F;
        v =  c     & 0x0F;

        compo_h[i] = h;
        compo_v[i] = v;

        if(max_h < h)
            max_h = h;
        if(max_v < v)
            max_v = v;

        compo_qt[i] = get_byte();
    }
}

// data restart interval
void CJPEGLS::dri(void)
{
    get_word();
    //printf("--- DRI ---\n");
    interval = get_word();
}

// define quantize table
void CJPEGLS::dqt(void)
{
    unsigned char c;
    int i,j,size;

    size = get_word() - 2;
    //printf("--- DQT ---\n");

    while(size>0) {

        c = get_byte();
        size--;
        j = c & 7;
        if(j > n_qt)
            n_qt = j;

        if((c>>3)){
            // 16 bit DQT
            for(i=0;i<64;i++){
                size-=2;
                qt[j][ i ] = (get_word()>>8);
            }
        }
        else{
            //  8 bit DQT
            for(i=0;i<64;i++){
                size--;
                qt[j][ i ] = get_byte();
            }
        }
    }
}

// define huffman table
void CJPEGLS::dht(void)
{
    unsigned tc,th;
    unsigned code = 0;
    unsigned char val;
    int i,j,k,num,Li[17];
    int len;
    HUFF *table;

    len = get_word() - 2;

    //printf("--- DHT ---\n");
    while(len > 0)
    {
        val = get_byte();

        tc = (val>>4) & 0x0F; // テーブルクラス(DC/AC成分セレクタ)
        th =  val     & 0x0F; // テーブルヘッダ(何枚目のプレーンか)

        table = (HUFF*)&(huff[tc][th]);

        num = 0;
        for (i = 1; i <= 16; i++) {
            Li[i] = get_byte();
            num += Li[i];
        }
        table->elem = num;

        // 符号生成
        k=0;
        for(i=1;i<=16;i++) {
            for(j=0;j<Li[i];j++) {
                table->size[k++] = i;
            }
        }

        k=0;
        code=0;
        i = table->size[0];
        while(k<num) {
            while(table->size[k] == i){
                table->code[k++] = code++;
            }
            if(k>=num)
                break;
            do{
                code = code << 1;
                i++;
            }while(table->size[k] != i);
        }

        for(k=0;k<num;k++)
            table->value[k] = get_byte();

        len = len - 18 - num;
    }
}

// start of scan
void CJPEGLS::sos(void)
{
    int i;
    unsigned char c;
    get_word();

    //printf("-------SOS----\n");
    scan_count = get_byte();

    for(i=0;i<scan_count;i++) {
        scan_id[i] = get_byte();

        c = get_byte();
        scan_dc[i] = c >> 4;   // DC Huffman Table
        scan_ac[i] = c & 0x0F; // AC Huffman Table
    }

    //3 bytes skip
    get_byte();
    get_byte();
    get_byte();
}

void CJPEGLS::init(void)
{
    int i;

    //printf("-------init----\n");

    idct_init();

    for(i=0;i<3;i++)
        mcu_preDC[i]=0;
    n_qt = 0;

    max_h = 0;
    max_v = 0;
    bit_remain = 0;
    bit_buff   = 0;
    // DRIリセット無し
    interval = 0;
}

int CJPEGLS::header(void)
{
    unsigned char c;
    int end = 0;
    while(!end)
    {
        c = get_byte();

        if(data_index >= data_size)
            return -1;

        c = get_byte();
        switch(c)
        {
        case 0xD8: break;
        case 0xD9: end = 1;break;
        case 0xC0: sof(); break;
        case 0xC4: dht(); break;
        case 0xDB: dqt(); break;
        case 0xDD: dri(); break;
        case 0xDA: sos(); end = 1;  break;
        default:
            skip();
        }
    }
    return 0;
}


// ------------------------------------ MCU decode --------------------------

// デコード
int CJPEGLS::decode_init(void)
{
    int i,j;

    for(i=0;i< scan_count;i++) {
        // i:scan
        for(j=0;j< compo_count;j++) {
            // j:frame
            if( scan_id[i] == compo_id[j]){
                scan_h[i] = compo_h[j];
                scan_v[i] = compo_v[j];
                scan_qt[i]= compo_qt[j];
                break;
            }
        }
        if(j >= compo_count){
            // プログレッシブJPEGだとこれが出ます
            return 1;
        }
    }

    mcu_width  = max_h * 8;
    mcu_height = max_v * 8;

    for(i=0;i<32*32*4;i++){
        mcu_buf[i] = 0x80;
    }
    for(i=0;i<scan_count;i++){
        mcu_yuv[i] = mcu_buf + i*32*32;
    }
    return 0;
}



// ハフマン 1シンボル復号
int CJPEGLS::get_huff(int tc,int th)
{
    HUFF *h = &(huff[tc][th]);
    int code,size,k,v;

    code = 0;
    size = 0;
    k = 0;

    while( size < 16 ) {
        size++;
        v = get_bits(1);
        code = (code << 1) | v;

        while(h->size[k] == size){
            if(h->code[k] == code){
                return h->value[k];
            }
            k++;
        }
    }

    //符号エラー
    return -1;
}

// 逆DCT（パク
// 浮動少数つかっちゃってるんで、どうにかしたい.

#ifdef USE_CHENDCT

#include "chendct.c"
void JPEG::idct_init(void)
{

}

void JPEG::idct(int *block,int *dest)
{
    ChenIDct(block,dest);
}
#else

#include "dct_tab"
// ８×８個 の『基底画像』生成
void CJPEGLS::idct_init(void)
{
    int i,j;
    for(i=0;i<64;i++)
        for(j=0;j<64;j++)
            base_img[i][j] = base_tab[i][j];
}

void CJPEGLS::idct(int *block,int *dest)
{
    int i,j;
    int temp[64];
    for(i=0;i<64;i++)
        temp[i] = 0;

    for(i=0;i<64;i++){
        if(block[i] != 0){
            for(j=0;j<64;j++) {
                temp[j] += block[i] * base_img[i][j];
            }
        }
    }
    for(i=0;i<64;i++)
        dest[i] = (int)( temp[i] / 4096);
}

#endif

// 符号化された数値を元に戻す

int CJPEGLS::get_value(int size) {
    int val = 0;
    if(size == 0)
        val = 0;
    else
    {
        val = get_bits(size);
        if (!(val & (1<<(size-1))))
            val = val - (1 << size) + 1;
    }
    return val;
}

// ---- ブロックのデコード ---
// ハフマンデコード＋逆量子化＋逆ジグザグ
int CJPEGLS::decode_huff(int scan,int *block)
{
    int size, val, run, index;
    int *pQt = (int *)(qt[scan_qt[scan]]);

    // DC
    size = get_huff(0,scan_dc[scan]);
    if(size < 0)
        return 0;

    val = get_value(size);
    mcu_preDC[scan] += val;
    block[0] = mcu_preDC[scan] * pQt[0];

    //AC復号
    index = 1;
    while(index<64)
    {
        size = get_huff(1,scan_ac[scan]);
        if(size < 0)
            break;
        // EOB
        if(size == 0)
            break;

        // RLE
        run  = (size>>4)&0xF;
        size = size & 0x0F;

        val = get_value(size);

        // ZRL
        while (run-- > 0)
            block[ zigzag_table[index++] ] = 0;

        block[ zigzag_table[index] ] = val * pQt[index];
        index++;
    }
    while(index<64)
        block[zigzag_table[index++]]=0;
    return 0;
}

// ブロック (補間かけるには、ここで)
// リサンプリング
#define INDEX(X,Y) (((Y)*8)+(X))
void CJPEGLS::mcu_bitblt(int *src,int *dest,int width,
                       int x0,int y0,int x1,int y1)
{
    int w,h,t;
    int x,y,x2,y2;
    int dx,dy;
    int c1,c2,c3,c4;
    int vx1,vx2;
    w = x1 - x0;
    h = y1 - y0;

    for(y=y0;y<y1;y++) {
        t = (y-y0) * 8;
        y2 = t / h;
        dy = t % h;
        for(x=x0;x<x1;x++) {
            t = (x-x0) * 8;
            x2 = t / w;
            dx = t % w;
            if(x2+1 > 7){
                c1 = src[INDEX(x2,y2)];
                c2 = c1;
                if(y2+1 > 7){
                    c3 = c1;
                    c4 = c1;
                } else {
                    c3 = src[INDEX(x2,y2+1)];
                    c4 = c3;
                }
            }
            else{
                c1 = src[INDEX(x2  ,y2)];
                c2 = src[INDEX(x2+1,y2)];
                if(y2+1 > 7){
                    c3 = c1;
                    c4 = c2;
                } else {
                    c3 = src[INDEX(x2  ,y2+1)];
                    c4 = src[INDEX(x2+1,y2+1)];
                }
            }
            vx1 = (c1 * (w-dx) + c2 * dx) / w;
            vx2 = (c3 * (w-dx) + c4 * dx) / w;
            dest[(y*width) + x] = (vx1 *(h-dy) + vx2 * dy) / h;
        }
    }
}
#undef INDEX
// MCU一個変換
void CJPEGLS::decode_mcu(void)
{
    int scan,val;
    int h,v;
    int *p,hh,vv;
    int block[64],dest[64];

    // mcu_width x mcu_heightサイズのブロックを展開
    for(scan=0;scan<scan_count;scan++)
    {
        hh = scan_h[scan];
        vv = scan_v[scan];
        //printf("scan %d (%dx%d)\n",scan,hh,vv);
        for(v=0;v<vv;v++){
            for(h=0;h<hh;h++){

                // ブロック(8x8)のデコード
                val = decode_huff(scan , block);

                // 逆DCT
                idct(block,dest);

                // 書き込みバッファ
                p = mcu_buf + (scan * 32 * 32);

                // 拡大転送
                mcu_bitblt(dest , p ,
                                mcu_width,
                                mcu_width * h / hh,
                                mcu_height* v / vv,
                                mcu_width * (h+1) / hh,
                                mcu_height* (v+1) / vv);
            }
        }
    }
}

// YCrCb=>RGB

void CJPEGLS::decode_yuv(int h,int v,unsigned char *rgb)
{
    int x0,y0,x,y,x1,y1;
    int *py,*pu,*pv;
    int Y,U,V,k;
    int R,G,B;
    int mw,mh,w;

    mw = mcu_width;
    mh = mcu_height;
    w = width;

    x0 = h * max_h * 8;
    y0 = v * max_v * 8;

    x1 = width - x0;
    if(x1 > mw)
        x1 = mw;
    y1 = height - y0;
    if(y1 > mh)
        y1 = mh;

    py = mcu_buf;
    pu = mcu_buf + 1024;
    pv = mcu_buf + 2048;


    for(y=0;y<y1;y++){
        for(x=0;x<x1;x++){
            k = y*mw+x;

            Y = py[ k ];
            U = pu[ k ];
            V = pv[ k ];

            R = 128 + ((Y*0x1000 + V*0x166E) / 4096);
            R = (R & 0xffffff00) ? (R >> 24) ^ 0xff : R;

            G = 128 + ((Y*0x1000 - V*0x0B6C) / 4096);
            G = (G & 0xffffff00) ? (G >> 24) ^ 0xff : G;

            B = 128 + ((Y*0x1000 - V*4 + U*0x1C59) / 4096);
            B = (B & 0xffffff00) ? (B >> 24) ^ 0xff : B;

            rgb[((y0+y)*w + (x0+x))*3  ] = R;
            rgb[((y0+y)*w + (x0+x))*3+1] = G;
            rgb[((y0+y)*w + (x0+x))*3+2] = B;
        }
    }
}

void CJPEGLS::decode(unsigned char *rgb)
{
    int h_unit,v_unit;
    int mcu_count,h,v;

    // MCUサイズ計算
    decode_init();

    h_unit = width / mcu_width;
    v_unit = height/ mcu_height;
    if((width  % mcu_width) > 0){
        h_unit++;
    }
    if((height % mcu_height) > 0){
        v_unit++;
    }

    // 1ブロック展開するかもしれない
    mcu_count = 0;
    for(v=0;v<v_unit;v++){
        for(h=0;h<h_unit;h++){
            mcu_count++;

            decode_mcu();
            decode_yuv(h,v,rgb);

            if(interval > 0 && mcu_count >= interval){

                // RSTmマーカをすっ飛ばす(FF hoge)
                // hogeは読み飛ばしてるので、FFも飛ばす

                bit_remain -= (bit_remain & 7);
                bit_remain -= 8;

                //printf("remain:%d\n",bit_remain);
                mcu_preDC[0] = 0;
                mcu_preDC[1] = 0;
                mcu_preDC[2] = 0;
                mcu_count = 0;
            }
        }
    }
}
