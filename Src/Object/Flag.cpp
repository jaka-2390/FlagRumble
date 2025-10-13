#include <DxLib.h>
#include"../Application.h"
#include "Flag.h"

Flag::Flag(void)
{
}

Flag::~Flag(void)
{
}

void Flag::Init(void)
{
	//ÉÇÉfÉãÇÃì«çû
	modelIdB_ = MV1LoadModel((Application::PATH_MODEL + "wood/Baby.mv1").c_str());

	scl_ = { 3.0f, 2.5f, 3.0f };							//ëÂÇ´Ç≥
	rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };			//âÒì]
	pos_ = { 2300.0f, 254.0f, 445.0f };						//à íu

}

void Flag::Update(void)
{
}

void Flag::Draw(void)
{
	MV1SetScale(modelIdB_, scl_);
	MV1SetRotationXYZ(modelIdB_, rot_);
	MV1SetPosition(modelIdB_, pos_);
	MV1DrawModel(modelIdB_);
}

VECTOR Flag::GetPosition() const
{
	return pos_;
}

void Flag::DrawCircleOnMap(VECTOR center, float radius, int color)
{
	const int div = 36;

	for (int i = 0; i < div; i++)
	{
		float t1 = (float)i / div;
		float t2 = (float)(i + 1) / div;

		float x1 = center.x + cosf(t1 * 2.0f * 3.14159265f) * radius;
		float z1 = center.z + sinf(t1 * 2.0f * 3.14159265f) * radius;

		float x2 = center.x + cosf(t2 * 2.0f * 3.14159265f) * radius;
		float z2 = center.z + sinf(t2 * 2.0f * 3.14159265f) * radius;

		float y = center.y;

		DrawLine3D(VGet(x1, y, z1), VGet(x2, y, z2), color);
	}
}
