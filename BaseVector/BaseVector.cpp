#include "BaseVector.h"

// �p�x����P�ʃx�N�g�����擾�ł���z�񏉊���
void BaseVector::InitBaseVector()
{
	const double c_dPai = 3.14159265358979323;			// �~����
	for (int nAngle = 0; nAngle < c_nMaxRound; nAngle++)
	{																				// �����
		double dAngle = c_dPai * 2 * nAngle / 360;			// ���W�A����
		pntBaseVector[nAngle].x = (float)cos(dAngle);		// �e����
		pntBaseVector[nAngle].y = (float)sin(dAngle);		// ���o��
	}
}
