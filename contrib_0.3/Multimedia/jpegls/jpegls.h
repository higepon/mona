/*
 * JPEG decoding engine for DCT-baseline
 *
 *      copyrights 2003 by nikq | nikq::club.
 */


#ifndef __JPEGLS_H_
#define __JPEGLS_H_

// ハフマンテーブル
typedef struct{
    int elem; //要素数
    unsigned short code[256];
    unsigned char  size[256];
    unsigned char  value[256];
}HUFF;

class CJPEGLS {

protected:

    // SOF
    int width;
    int height;

    // MCU
    int mcu_width;
    int mcu_height;

    int max_h,max_v;
    int compo_count;
    int compo_id[3];
    int compo_sample[3];
    int compo_h[3];
    int compo_v[3];
    int compo_qt[3];

    // SOS
    int scan_count;
    int scan_id[3];
    int scan_ac[3];
    int scan_dc[3];
    int scan_h[3];  // サンプリング要素数
    int scan_v[3];  // サンプリング要素数
    int scan_qt[3]; // 量子化テーブルインデクス

    // DRI
    int interval;

    int mcu_buf[32*32*4]; //バッファ
    int*mcu_yuv[4];
    int mcu_preDC[3];

    // DQT
    int qt[3][64];
    int n_qt;

    // 基底画像 ( [横周波数uπ][縦周波数vπ][横位相(M/8)][縦位相(N/8)]
    int base_img[64][64];

    // DHT
    HUFF huff[2][3];


    // i/o
    unsigned char *data;
    int data_index;
    int data_size;

    unsigned long bit_buff;
    int bit_remain;

    unsigned char  get_byte(void);
    unsigned short get_word(void);
    unsigned short get_bits(int bit);

    void init(void);

    // ヘッダ関係
    void skip(void); // 未実装セグメント
    void sof(void); // Start of Frame
    void sos(void); // Start Of Scan
    void dri(void); // Define Restart Interval
    void dqt(void); // Define Quantize Table
    void dht(void); // Define Huffman Table
    int header(void);

    // デコード関係
    int decode_init(void);
    int get_huff(int tc,int th);
    int get_value(int size);
    int decode_huff(int scan,int *block);
    void idct_init(void);
    void idct(int *,int *);

    // MCU
    void mcu_bitblt(int *src,int *dest,int width,
                    int x0,int y0,int x1,int y1);
    void decode_mcu(void);
    void decode_yuv(int h,int v,unsigned char *rgb);
    void decode(unsigned char *rgb);

public:

    //インターフェイス
    CJPEGLS();
    ~CJPEGLS();
    int Open(unsigned char *data,int size);
    void GetInfo(int *width,int *height);
    void Decode(unsigned char * dest);
};


#endif
