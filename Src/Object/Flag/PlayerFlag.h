#pragma once
#include "../FlagBase.h"

class PlayerFlag : public FlagBase
{
public:

	PlayerFlag(VECTOR pos, ENEMY_TYPE type, STATE state);	//コンストラクタ
	~PlayerFlag(void);						//デストラクタ

	void Init(void);												//初期化処理
	void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);	//更新処理
	void Draw(void);												//描画処理

private:

};

