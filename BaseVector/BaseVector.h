#ifndef H_BASE_VECTOR____
#define H_BASE_VECTOR____

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct TPointf
{
	float x,y;
} POINTF;

const int c_nMaxRound = 360;
namespace BaseVector
{																// 名前空間の指定
	static POINTF  pntBaseVector[c_nMaxRound];	// 単位ベクトル格納配列
	void    InitBaseVector();							// 単位ベクトルの初期化
}

/*
	単位ベクトルの取得
	in:  int     = 角度 (0～359)
	out: POINTF* = 単位ベクトル構造体のポインタ
*/
#define GET_BASE_VECTOR(x)  (&BaseVector::pntBaseVector[(x)])


#endif