#include <DxLib.h>
#include<EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
#include"../../Application.h"
#include "PlayerFlag.h"

PlayerFlag::PlayerFlag(VECTOR pos, ENEMY_TYPE type, STATE state) 
	: FlagBase(pos, type, state)
{
}

PlayerFlag::~PlayerFlag(void)
{
}

void PlayerFlag::Init(void)
{
	FlagBase::Init();
}

void PlayerFlag::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	FlagBase::Update(playerPos, enemies);
}

void PlayerFlag::Draw(void)
{
	FlagBase::Draw();
}
