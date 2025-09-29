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
	pos_ = { 0.0f, -3.5f, 0.0f };							//à íu

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
