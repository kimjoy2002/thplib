#include "BaseVector.h"

// 角度から単位ベクトルを取得できる配列初期化
void BaseVector::InitBaseVector()
{
	const double c_dPai = 3.14159265358979323;			// 円周率
	for (int nAngle = 0; nAngle < c_nMaxRound; nAngle++)
	{																				// 一周分
		double dAngle = c_dPai * 2 * nAngle / 360;			// ラジアンに
		pntBaseVector[nAngle].x = (float)cos(dAngle);		// 各軸を
		pntBaseVector[nAngle].y = (float)sin(dAngle);		// 取り出す
	}
}
