
Code:Mones Monaにわかネットワーク対応 Ver0.32

依存:Monaコア Ver0.30
     contrib_0.3/Sysutils/libPci

1.ビルド方法。
 (1)事前に、Monaコアをビルドする。
    MonaNew(仮) を、CVSからとってくるなりして、適当な場所に置く
    
    export MONADIR=/usr/local/mona
    cd MonaNew
    make

 (2)monesのビルド
    cygwinで、このファイルのあるディレクトリに移動し
    make
 
 (3)ISOイメージの作成
    MonaNew/tools/mkimg  へ移動し
    make



2.ツリー説明

yamami_mones  Mones:ルート
 +--Makefile
 |  
 +--monesoc  :Mona Socketライブラリ (UDPのみサポート) 
 |   
 |  
 +--netprot  :ネットワークプロトコルサーバ(依存:nicDriver,monesoc)
 |           EtherNet
 |           ARP
 |           ICMP
 |           IP
 |           UDP
 |           TCP (未実装)
 |                 2006/03/23 追記 Code:Mones Rewrite2006 lwipを移植にリライト予定
 |  
 +--nicdrv  :NICドライバ (依存:pci)
 |   |  
 |   +--AbstractMonic.cpp  :抽象NICドライバ
 |   |  
 |   +--MonesLoader.cpp   : NICドライバローダ
 |   |  
 |   +--ne2000 :NE2000ドライバ
 |       |  
 |       +--Ne2000MoNic.cpp
 |       |  
 |       +--Ne2000MoNic.h
 |    
 |    ・・・AbstractMonic(抽象クラス) のインターフェース実装で、Monesとしては動作する。
 |  
 |  
 +--sampleApp  : Monesサンプルアプリ
 |   |  
 |   +--yamami : Yamami Mones試験用アプリ
 |       |  
 |       |  
 |       +--main.cpp
 



3.動作させる方法

 2005/08/09現在。対応NICがNE2000互換アダプタのみなので
 事実上、エミュレータのQEMU か Bochs のみでの動作となります。
  (NE2000互換アダプタの実機なら別ですが、今時ありそうも無い)
 
 詳細は、YamamiのWiki
 Mona製作所/ネットワーク対応/Mones設定方法 
 http://f38.aaa.livedoor.jp/~yamami/pukiwiki/pukiwiki.php?%5B%5BMona%C0%BD%BA%EE%BD%EA%2F%A5%CD%A5%C3%A5%C8%A5%EF%A1%BC%A5%AF%C2%D0%B1%FE%2FMones%C0%DF%C4%EA%CA%FD%CB%A1%5D%5D
 
 とりあえずMonaが、ping応答くらいはします。

