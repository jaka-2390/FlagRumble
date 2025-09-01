#pragma once
#include "ActorBase.h"
#include <memory>
#include <vector>

class Player;
class Tree;
class Collider;

class Item : public ActorBase
{
public:
	//初期化用 & デバッグ
	static constexpr float ZERO = 0.0f;

	static constexpr float ITEM_MODEL_SCALE = 0.1f;

	const float ITEM_GROUND_Y = 2.0f;
	const float ITEM_MODEL_BOTTOM_OFFSET = 3.0f;

	static constexpr int DEBUG_SPHERE_DIV = 8;

	static constexpr int COLOR_WHITE = 0xffffff;
	static constexpr int COLOR_BLUE = 0x0000ff;

	//敵の種類
	enum class TYPE
	{
		NONE,
		WATER,	//水
		POWER,	//パワーアップ
		SPEED,	//スピードアップ
		HEAL,	//ヒール
		MUTEKI,	//無敵
		ALL,	//全部
		MAX
	};

	// コンストラクタ
	Item(Player& player, const Transform& transform, TYPE itemType, Tree& tree);

	// デストラクタ
	~Item(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	VECTOR GetPos(void);		// 座標の取得
	void SetPos(VECTOR pos);	// 座標の設定

	void SetCollisionPos(const VECTOR collision);//衝突判定用の球体
	VECTOR GetCollisionPos(void)const;		// 衝突用の中心座標の取得
	float GetCollisionRadius(void);		// 衝突用の球体半径の取得

	void SetIsAlive(bool isAlive);
	bool GetIsAlive();

	void SetScale(float scale);			//itemのスケールを設定
	void Respawn(const VECTOR& newPos);	//itemを再利用

	TYPE GetItemType(void) const;

	void DrawDebug(void);	//デバッグ用

private:

	Player& player_;
	Tree& tree_;
	TYPE itemType_;	//ドロップするアイテムの種類

	int modelId_;//モデルの格納

	VECTOR pos_;	// 表示座標

	bool isAlive_;

	float floatTimer_ = 0.0f;  // アニメーション用のタイマー
	float floatHeight_ = 20.0f; // 上下に動く振幅（高さ）
	float floatSpeed_ = 4.0f;   // 動きの速さ
	float baseY_ = 0.0f;

	float collisionRadius_;		// 衝突判定用の球体半径
	VECTOR collisionLocalPos_;	// 衝突判定用の球体中心の調整座標

	// 衝突判定
	void Collision(void);
	
	void InitModel(void);

	void ItemUse(void);	//アイテムの効果事の処理
};