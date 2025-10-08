#pragma once
#include <memory>
#include "..//Application.h"
#include "SceneBase.h"
#include "string"

class SceneManager;

class DemoScene :public SceneBase
{

public:

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int gray = 0xaaaaaa; //灰
	int shadow = 0x888888; //影

	//テキスト関連
	static constexpr int TXT = -2;									//テキストの初期化
	static constexpr int FONT_SIZE = 38;							//文字のサイズ
	static constexpr int START_TXT_X = 385;							//テキストの最初X
	static constexpr int START_TXT_Y = 900;							//テキストの最初Y
	static constexpr int FRAME_X = Application::SCREEN_SIZE_X / 2;	//枠のX
	static constexpr int FRAME_Y = Application::SCREEN_SIZE_Y - 175;	//枠のY
	static constexpr int FRAME_SIZE = 0.8f;							//枠の大きさ
	static constexpr int UP_BLACK = 800;							//幕の位置
	static constexpr int DOWN_BLACK = 980;							//幕の位置
	static constexpr int CNT = 90;									//カウントの最大
	static constexpr int TRI_SIZE = 1.2f;							//三角の大きさ
	static constexpr int TRI_POS_X = 1490;							//三角の位置
	static constexpr int TRI_POS_UY = 985;							//三角の位置
	static constexpr int TRI_POS_DY = 975;							//三角の位置

	//スポットライト関連
	static constexpr int LIGHT_RX = 1300;		//右の光
	static constexpr int LIGHT_LX = 500;		//左の光
	static constexpr int UP_LIGHT = 695;		//上の光
	static constexpr int DOWN_LIGHT = 700;		//下の光
	static constexpr int UP_SHADOW_X = 90;		//上の影　半径X
	static constexpr int UP_SHADOW_Y = 30;		//上の影　半径Y
	static constexpr int DOWN_SHADOW_X = 150;	//下の影　半径X
	static constexpr int DOWN_SHADOW_Y = 40;	//下の影　半径Y
	static constexpr int TREE_POS = 630;		//木の位置
	static constexpr int WATER_POS = 655;		//水の位置
	static constexpr int PLAYER_POS = 650;		//自の位置
	static constexpr int TREE_SIZE = 0.35f;		//木の大きさ
	static constexpr int WATER_SIZE = 7.0f;		//水の大きさ
	static constexpr int PLAYER_SIZE = 3.5f;	//自の大きさ

	//案内役関連
	static constexpr int FRONT = 0;											//前向き
	static constexpr int RIGHT = 1;											//右向き
	static constexpr int LEFT = 2;											//左向き
	static constexpr int UP = 3;											//上向き
	static constexpr int DOWN = 4;											//下向き
	static constexpr int GOD_POS_X = Application::SCREEN_SIZE_X / 2;		//案内人の位置X
	static constexpr int GOD_POS_Y = Application::SCREEN_SIZE_Y / 2 - 200;	//案内人の位置Y

	enum class TXT
	{
		T1,
		T2,
		T3,
		T4,
		T5,
		T6,
		T7,
		T8,
		T9,
		T10,
		T11,
		T12,
		T13,
		T14,
		T15,
		T16,
		T17,
		T18,
		T19,
		T20,
		T21,
		MAX
	};

	DemoScene(void);     //コンストラクタ
	~DemoScene(void);    //デストラクタ

	void Init(void)override;      //初期化処理
	void Update(void)override;    //更新処理
	void Draw(void)override;      //描画処理
	void Release(void)override;

private:

	SceneManager* sceneManager_;   //シーンマネジャーのポインタ格納領域
	int demoSound_;

	int tree_;
	int player_;
	int water_;
	int god_[5];
	int gNo_;
	int waku_;
	int tri_;

	int txt_;
	int cnt_;
	int now_;
	int old_;

};