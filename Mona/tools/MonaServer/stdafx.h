// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#ifndef __STDAFX_H__
#define __STDAFX_H__

#include <sys/types.h>
#include <sys/List.h>
#include <sys/BinaryTree.h>
#include <monapi/Array.h>
#include "BitMap.h"
#include "info.h"
#include "kernel.h"
#include "vbe.h"
#include "VirtualConsole.h"
#include "VesaConsole.h"
#include "global.h"
#include "kernel.h"
#include "Node.h"
extern "C" char* ltona(long value, char* str, int n, int base);

#endif
