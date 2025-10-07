#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/GravityManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Common/Transform.h"
#include "Camera.h"

Camera::Camera(void)
{
	angles_ = VECTOR();
	cameraUp_ = VECTOR();
	mode_ = MODE::NONE;
	pos_ = AsoUtility::VECTOR_ZERO;
	targetPos_ = AsoUtility::VECTOR_ZERO;
	followTransform_ = nullptr;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ChangeMode(MODE::FIXED_POINT);
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{
	// カメラの遠近設定（これがないと描画がおかしくなる）
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	// ポーズ中は追従や回転の更新をスキップ
	if (!isPaused_) {
		switch (mode_)
		{
		case Camera::MODE::FIXED_POINT:
			SetBeforeDrawFixedPoint();
			break;
		case Camera::MODE::FOLLOW:
			SetBeforeDrawFollow(); // pos_, targetPos_, cameraUp_ を更新
			break;
		}
	}

	// 現在のカメラ位置・向きで描画設定
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		cameraUp_
	);

	// Effekseer のカメラ同期
	Effekseer_Sync3DSetting();
}

void Camera::Draw(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

VECTOR Camera::GetTargetPos(void) const
{
	return targetPos_;
}

Quaternion Camera::GetQuaRot(void) const
{
	return rot_;
}

Quaternion Camera::GetQuaRotOutX(void) const
{
	return rotOutX_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, pos_));
}

float Camera::GetHorizontalAngleRad(void) const
{
	return angles_.y;
}

void Camera::ChangeMode(MODE mode)
{
	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FOLLOW:
		break;
	}
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	// カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

	angles_.x = AsoUtility::Deg2RadF(30.0f);
	angles_.y = 0.0f;
	angles_.z = 0.0f;

	rot_ = Quaternion();
}

void Camera::SyncFollow(void)
{
	auto& gIns = GravityManager::GetInstance();

	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// 重力の方向制御に従う
	Quaternion gRot = gIns.GetTransform().quaRot;

	// 正面から設定されたY軸分、回転させる
	rotOutX_ = gRot.Mult(Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y));

	// 正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	// 注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS);
	targetPos_ = VAdd(pos, localPos);

	// カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS);
	pos_ = VAdd(pos, localPos);

	// カメラの上方向
	cameraUp_ = gRot.GetUp();
}

void Camera::ProcessRot(void)
{
	auto& ins = InputManager::GetInstance();

	if (ins.IsNew(KEY_INPUT_UP))
	{
		angles_.x += AsoUtility::Deg2RadF(1.5);
		if (angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= AsoUtility::Deg2RadF(1.5);
		if (angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		angles_.y += AsoUtility::Deg2RadF(1.5);
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		angles_.y -= AsoUtility::Deg2RadF(1.5);
	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
}

void Camera::SetBeforeDrawFollow(void)
{
	// カメラ操作
	ProcessRot();

	// 追従対象との相対位置を同期
	SyncFollow();
}

void Camera::SetBeforeDrawSelfShot(void)
{
}
