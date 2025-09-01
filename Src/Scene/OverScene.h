#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SceneManager;
class SkyDome;
class AnimationController;

class OverScene : public SceneBase
{
public:

	struct VECTOR4
	{
		float x, y, z, w;
		VECTOR4() : x(0), y(0), z(0), w(0) {}
		VECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	};

	OverScene(void);	// コンストラクタ
	~OverScene(void);	// デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
private:
	int cnt;

	// 画像
	int imgGameOver_;
	int imgCursor_[2];
	int imgDieTree_;
	int imgLightCircle_;	// 光

	// メニュー
	int selectedIndex_;       // 選択中のメニューインデックス（0:リプレイ, 1:タイトルへ）
	bool isMenuActive_;

	// 死んでしまった…
	int maskLeftX_;   // 黒帯の左端X座標
	int maskRightX_;  // 黒帯の右端X座標（固定）

	// カウンター
	int cheackCounter_;

	static constexpr int maskWidthMax_ = 1600;  // 画像横幅
	static constexpr int revealSpeed_ = 4;

	// キャラクター
	Transform charactor_;

	// アニメーション
	std::unique_ptr<AnimationController> animationController_;

	// 3D→2D変換用補助関数
	VECTOR4 MulMatVec(const MATRIX& m, const VECTOR4& v);
	bool WorldToScreen(const VECTOR& worldPos, VECTOR& screenPos);
};