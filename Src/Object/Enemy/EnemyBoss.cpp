#include "EnemyBoss.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyBoss::EnemyBoss() :EnemyBase()
{
}

void EnemyBoss::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/Boss/Boss.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE,		path, 20.0f, 2);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, 20.0f, 4);
	animationController_->Add((int)ANIM_TYPE::ATTACK,	path, 15.0f, 0);
	animationController_->Add((int)ANIM_TYPE::DEATH,	path, 20.0f, 1);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyBoss::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BOSS));

	transform_.scl = { 1.0f, 1.0f, 1.0f };						// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(180.0f), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { 0.0f, 0.0f, 0.0f };						// 右方向に移動する

	speed_ = 2.0f;		// 移動スピード

	isAlive_ = true;	// 初期は生存状態

	hp_ = BOSS_MAX_HP;	// HPの設定

	attackPow_ = 3;

	collisionRadius_ = 200.0f;	// 衝突判定用の球体半径
	collisionLocalPos_ = { 0.0f, 60.0f, 0.0f };	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = 200.0f;		// 攻撃判定用と攻撃範囲の球体半径

	enemyType_ = TYPE::BOSS;

	// 初期状態
	ChangeState(STATE::PLAY);
}

Item::TYPE EnemyBoss::GetDropItemType() const
{
	return Item::TYPE::ALL;
}

void EnemyBoss::DrawBossHpBar(void)
{
	if (!isAlive_)
	{
		return;
	}

	int barX = (Application::SCREEN_SIZE_X - BOSS_HP_BAR_WIDTH) / 2; // 中央X
	int barY = BOSS_HP_BAR_Y; // 上から80px

	float hpRate = static_cast<float>(hp_) / BOSS_MAX_HP;
	int hpDrawWidth = static_cast<int>(BOSS_HP_BAR_WIDTH * hpRate);

	// ラベル
	SetFontSize(55);
	DrawFormatString(barX + BOSS_LABEL_OFFSET_X, barY + BOSS_LABEL_OFFSET_Y, GetColor(255, 255, 255), "BOSS");
	SetFontSize(16);

	// 背景バー（黒）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, GetColor(0, 0, 0), TRUE);

	// HPバー（赤）
	DrawBox(barX, barY, barX + hpDrawWidth, barY + BOSS_HP_BAR_HEIGHT, GetColor(255, 0, 0), TRUE);

	// 枠線（白）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, GetColor(255, 255, 255), FALSE);
}