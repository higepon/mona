Mesa-6.2.1をMona用にコンパイルするためのコンパイル設定ファイルとMakefile

必要なもの
MonaのCVSソース
MesaLib-6.2.1.tar.bz2
tmpmonalibc-3.tar.gz

インストール方法
export MONADIR=Monaのソースツリーの場所
mkdir work
cd work
tar xjvf Mesa-6.2.1.tar.bz2
tar xzvf MesaForMona-3.tar.gz
tar xzvf tmpmonalibc-3.tar.gz
cd MesaForMona
cp -r * ../Mesa-6.2.1
cd ../Mesa-6.2.1

1. 静的ライブラリ
make mona-static
lib/libOSMesa.a, libGLU.a
ができる。

2. 動的ライブラリ
make mona
lib/OSMESA.DLL, libOSMesa-imp.a, GLU.DLL, libGLU-imp.a
ができる。
