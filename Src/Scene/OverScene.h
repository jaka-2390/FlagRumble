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

	static constexpr int TWO = 2;
	static constexpr float VALUE_TWO = 2.0f;

	// ゲームオーバー文字列
	static constexpr int GAMEOVER_IMG_X_OFFSET = 555;
	static constexpr int GAMEOVER_IMG_Y = 50;
	static constexpr int MASSAGE_X_OFFSET = 400;

	// プレイヤーモデル光の描画
	static constexpr VECTOR PLAYER_POS = { 0.0f, -200.0f, 0.0f };
	static constexpr VECTOR PLAYER_SIZE = { 1.4f, 1.4f, 1.4f };
	static constexpr float LIGHT_SCALE_X = 1.3f;
	static constexpr float LIGHT_SCALE_Y = 2.05f;
	static constexpr int LIGHT_OFFSET_Y = 200;
	static constexpr int LIGHT_ALPHA = 55;

	//アニメーション速度
	static constexpr float ANIM_SPEED = 20.0f;

	// メニュー文字
	static constexpr int MENU_FONT_SIZE = 55;
	static constexpr int MENU_PLAY_Y = 800;
	static constexpr int MENU_TITLE_Y = 890;
	static constexpr int MENU_POS_X = 1500;

	// ストーリー文字
	static constexpr int STORY_FONT_SIZE = 90;
	static constexpr int STORY_X_OFFSET = 675;
	static constexpr int STORY_Y = 330;
	static constexpr int BLACK_BOX_SLIDE_SPEED = 4;

	// 黒帯
	static constexpr int BLACK_BOX_X1 = 200;
	static constexpr int BLACK_BOX_Y1 = 300;
	static constexpr int BLACK_BOX_X2 = 1700;
	static constexpr int BLACK_BOX_Y2 = 500;

	static constexpr int FORCE_TITLE_TIME = 3600; // 60秒でタイトルに戻る

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int yellow = 0xffff00;//黄

	//struct VECTOR4
	//{
	//	float x, y, z, w;
	//	VECTOR4() : x(0), y(0), z(0), w(0) {}
	//	VECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	//};

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
	bool WorldToScreen(const VECTOR& worldPos, VECTOR& screenPos);

};