#include "FlagBase.h"

VECTOR FlagBase::GetPosition() const
{
    return pos_;
}

void FlagBase::DrawCircleOnMap(VECTOR center, float radius, int color)
{
	const int div = 36;

	for (int i = 0; i < div; i++)
	{
		float t1 = (float)i / div;
		float t2 = (float)(i + 1) / div;
		float x1 = center.x + cosf(t1 * 2.0f * DX_PI_F) * radius;
		float z1 = center.z + sinf(t1 * 2.0f * DX_PI_F) * radius;
		float x2 = center.x + cosf(t2 * 2.0f * DX_PI_F) * radius;
		float z2 = center.z + sinf(t2 * 2.0f * DX_PI_F) * radius;

		DrawLine3D(VGet(x1, center.y, z1), VGet(x2, center.y, z2), color);
	}
}

void FlagBase::DrawGauge3D(VECTOR center, float gaugeRate)
{
	// ゲージバーを円の上（少し高い位置）に表示
	VECTOR gaugePos = VAdd(center, VGet(0.0f, 80.0f, 0.0f)); // 円より上
	VECTOR screenPos = ConvWorldPosToScreenPos(gaugePos);

	int barWidth = 100;
	int barHeight = 10;

	int x = (int)screenPos.x - barWidth / 2;
	int y = (int)screenPos.y - 20; // 少し上にオフセット

	// 外枠
	DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, GetColor(255, 255, 255), FALSE);

	// 中身
	int fillWidth = (int)(barWidth * gaugeRate);
	DrawBox(x, y, x + fillWidth, y + barHeight, GetColor(0, 255, 0), TRUE);
}
