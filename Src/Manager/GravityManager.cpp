#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Planet.h"
#include "../Object/Player.h"
#include "../Object/EnemyBase.h"
#include "GravityManager.h"

GravityManager* GravityManager::instance_ = nullptr;

GravityManager::GravityManager(void)
{
	player_ = nullptr;
	enemyBase_ = nullptr;
	preDirGravity_ = AsoUtility::DIR_D;
	dirGravity_ = AsoUtility::DIR_D;
	dirUpGravity_ = AsoUtility::DIR_U;
	lastHitNormal_ = AsoUtility::DIR_U;
	lastHitPos_ = AsoUtility::VECTOR_ZERO;
	slerpPow_ = 0.0f;
	step_ = 0.0f;

	state_ = STATE::IDLE;
}

void GravityManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new GravityManager();
	}
	instance_->Init();
}

GravityManager& GravityManager::GetInstance(void)
{
	return *instance_;
}

void GravityManager::Init(void)
{
	dirGravity_ = AsoUtility::DIR_D;
	preDirGravity_ = AsoUtility::VECTOR_ZERO;
	step_ = -1.0f;
	slerpPow_ = DEFAULT_POW_SLERP;

	transform_.quaRot = Quaternion();
	transform_.Update();
}

void GravityManager::Update(void)
{
 	switch (state_)
	{
	case GravityManager::STATE::IDLE:
		UpdateIdle();
		break;
	case GravityManager::STATE::STAGE_CHANGE:
		UpdateChangeStage();
		break;
	}

	// 重力方向や、正方向の計算
	Calculate();
}

void GravityManager::UpdateIdle(void)
{
}

void GravityManager::UpdateChangeStage(void)
{
}

void GravityManager::Destroy(void)
{
	delete instance_;
}

void GravityManager::Calculate(void)
{
	// 重力方向
	dirGravity_ = CalcDirGravity();
	
	// 重力の反対方向(ジャンプ方向)
	dirUpGravity_ = VScale(dirGravity_, -1.0f);

	// 現在の上方向(つまり、重力の反対方向)
	VECTOR up = transform_.GetUp();

	// ２つのベクトル間の回転量(差)を求める
	Quaternion rot = Quaternion::FromToRotation(up, dirUpGravity_);

	slerpPow_ = 0.1f;

	// 求めた回転量で、現在の重力制御を回転させる(差が埋まる)
	Quaternion goal = rot.Mult(transform_.quaRot);
	//from = Quaternion::Slerp(from, to, 0.1f);
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, goal, slerpPow_);
}

std::weak_ptr<Planet> GravityManager::GetActivePlanet(void) const
{
	return activePlanet_;
}

void GravityManager::ChangeActivePlanet(std::weak_ptr<Planet> planet)
{
	// 新しい惑星をセット
	activePlanet_ = planet;
}

void GravityManager::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void GravityManager::SetEnemyBase(std::shared_ptr<EnemyBase> enemy)
{
	enemyBase_ = enemy;
}

const Transform& GravityManager::GetTransform(void) const
{
	return transform_;
}

VECTOR GravityManager::GetDirGravity(void) const
{
	return dirGravity_;
}

VECTOR GravityManager::GetDirUpGravity(void) const
{
	return dirUpGravity_;
}

float GravityManager::GetPower(void) const
{
	float ret = 0.0f;
	if (activePlanet_.lock() != nullptr)
	{
		ret = activePlanet_.lock()->GetGravityPow();
	}
	return ret;
}

Quaternion GravityManager::GetPreQuaRot(void) const
{
	return preDirGravity_;
}

void GravityManager::SetLastHitNormal(const VECTOR& normal)
{
	lastHitNormal_ = normal;
}

void GravityManager::SetLastHitPos(const VECTOR& pos)
{
	lastHitPos_ = pos;
}

VECTOR GravityManager::GetLastHitPos(void) const
{
	return lastHitPos_;
}

void GravityManager::ChangeState(STATE state)
{
	state_ = state;
	switch (state_)
	{
	case GravityManager::STATE::IDLE:
		break;
	case GravityManager::STATE::STAGE_CHANGE:
		break;
	}
}

VECTOR GravityManager::CalcDirGravity(void) const
{
	VECTOR ret = AsoUtility::DIR_D;

	if (activePlanet_.lock() == nullptr || player_ == nullptr)
	{
		return ret;
	}

	if (activePlanet_.lock() == nullptr || enemyBase_ == nullptr)
	{
		return ret;
	}

	// ステージタイプによって、重力方向の計算を変える
	Planet::TYPE stageType = activePlanet_.lock()->GetType();
	switch (stageType)
	{
	case Planet::TYPE::GROUND:
		ret = AsoUtility::DIR_D;
		break;
	case Planet::TYPE::SPHERE:
	{
		VECTOR Planet_ = activePlanet_.lock()->GetTransform().pos;
		ret = VNorm(VSub(Planet_, player_->GetTransform().pos));
	}
		break;
	case Planet::TYPE::TRANS_ROT:
		break;
	case Planet::TYPE::TRANS_CAMERA_FIXED:
		break;
	case Planet::TYPE::ROAD:
		break;
	}

	return ret;
}
