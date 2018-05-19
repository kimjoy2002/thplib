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
{																// ���O��Ԃ̎w��
	static POINTF  pntBaseVector[c_nMaxRound];	// �P�ʃx�N�g���i�[�z��
	void    InitBaseVector();							// �P�ʃx�N�g���̏�����
}

/*
	�P�ʃx�N�g���̎擾
	in:  int     = �p�x (0�`359)
	out: POINTF* = �P�ʃx�N�g���\���̂̃|�C���^
*/
#define GET_BASE_VECTOR(x)  (&BaseVector::pntBaseVector[(x)])


#endif