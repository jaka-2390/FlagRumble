#include<DxLib.h>
#include"../Application.h"
#include"../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include"../Manager/SoundManager.h"
#include"DemoScene.h"

DemoScene::DemoScene(void)
{
}
DemoScene::~DemoScene(void)
{
}

void DemoScene::Init(void)
{
	old_ = 0;
	now_ = 0;
	demoSound_ = LoadMusicMem("sound/SE/set.mp3");
	waku_ = LoadGraph("Data/Image/Demo/waku.png");
	tree_ = LoadGraph("Data/Image/Demo/tree.png");
	water_ = LoadGraph("Data/Image/Demo/water.png");
	player_ = LoadGraph("Data/Image/Demo/player.png");
	god_[FRONT] = LoadGraph("Data/Image/Demo/God/god.png");
	god_[RIGHT] = LoadGraph("Data/Image/Demo/God/goda.png");
	god_[LEFT] = LoadGraph("Data/Image/Demo/God/godb.png");
	god_[UP] = LoadGraph("Data/Image/Demo/God/godc.png");
	god_[DOWN] = LoadGraph("Data/Image/Demo/God/godd.png");
	tri_ = LoadGraph("Data/Image/Demo/tri.png");
	gNo_ = 0;
	txt_ = TXT;
	cnt_ = 0;

	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::DEMO_BGM, Sound::TIMES::LOOP);
}

void DemoScene::Update(void)
{
	InputManager& ins = InputManager::GetInstance();

	old_ = now_;
	now_ = CheckHitKey(KEY_INPUT_RETURN);

	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::FORCE_ONCE);
	}

	if (old_ == 0 && now_ == 1)
	{
		PlayMusicMem(demoSound_, DX_PLAYTYPE_BACK);
		txt_++;
	}
	if (txt_ == static_cast<int>(TXT::MAX) - 1)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

#pragma region god eye
	if (txt_ >= 1)gNo_ = RIGHT;
	if (txt_ >= 2)gNo_ = FRONT;
	if (txt_ >= 4)gNo_ = DOWN;
	if (txt_ >= 5)gNo_ = FRONT;
	if (txt_ >= 7)gNo_ = LEFT;
	if (txt_ >= 8)gNo_ = FRONT;
	if (txt_ >= 9)gNo_ = DOWN;
	if (txt_ >= 10)gNo_ = FRONT;
	if (txt_ >= 13)gNo_ = UP;
	if (txt_ >= 14)gNo_ = FRONT;
	if (txt_ >= 15)gNo_ = LEFT;
	if (txt_ >= 16)gNo_ = FRONT;
#pragma endregion

}

void DemoScene::Draw(void)
{
	cnt_++;
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);//黒で覆う

#pragma region テキスト
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * -1 - FONT_SIZE * txt_, "使徒諸君おはよう　神だ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 0 - FONT_SIZE * txt_, "私を知っている？何を寝ぼけている。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 1 - FONT_SIZE * txt_, "お前は生まれ堕ちたばかりだろう。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 2 - FONT_SIZE * txt_, "まぁいい うまれてすぐだろうが神託だ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 3 - FONT_SIZE * txt_, "お前にはある使命を与える。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 4 - FONT_SIZE * txt_, "これから生まれる世界の礎となる植物", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 5 - FONT_SIZE * txt_, "「ユグドラシル」の守護と育成だ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 6 - FONT_SIZE * txt_, "植物の生命力を狙うモンスターを倒し、", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 7 - FONT_SIZE * txt_, "その生命力を奪って植物に注ぎ込め。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 8 - FONT_SIZE * txt_, "戦い方がわからない？", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 9 - FONT_SIZE * txt_, "お前には私の力の一部を渡してある。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 10 - FONT_SIZE * txt_, "矢印キーで周囲を見渡して敵を見つけて", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 11 - FONT_SIZE * txt_, "WASDキーで近づいてEキーで攻撃しろ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 12 - FONT_SIZE * txt_, "左Shiftを使えば少しは早く移動できる。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 13 - FONT_SIZE * txt_, "あぁ　それと、", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 14 - FONT_SIZE * txt_, "敵の攻撃で倒れても私の加護で蘇らせる。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 15 - FONT_SIZE * txt_, "だから何があっても成し遂げろ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 16 - FONT_SIZE * txt_, "神の使徒たるお前に失敗は許されない。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 17 - FONT_SIZE * txt_, "しくじれば神罰だ。", white);
	DrawString(START_TXT_X, START_TXT_Y + FONT_SIZE * 18 - FONT_SIZE * txt_, "頼んだぞ。", white);
#pragma endregion

	DrawBox(0, 0, Application::SCREEN_SIZE_X, UP_BLACK, black, true);
	DrawBox(0, DOWN_BLACK, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);

	if (cnt_ % CNT <= CNT / 2)DrawRotaGraph(TRI_POS_X, TRI_POS_DY, TRI_SIZE, 0, tri_, true);
	else(DrawRotaGraph(TRI_POS_X, TRI_POS_UY, TRI_SIZE, 0, tri_, true));

	DrawRotaGraph(GOD_POS_X, GOD_POS_Y, 1, 0, god_[gNo_], true);

	if (txt_ > 3 && txt_ < 6)
	{
		DrawOval(LIGHT_RX, DOWN_LIGHT, DOWN_SHADOW_X, DOWN_SHADOW_Y, gray, true);
		DrawOval(LIGHT_RX, UP_LIGHT, UP_SHADOW_X, UP_SHADOW_Y, shadow, true);
		DrawRotaGraph(LIGHT_RX, TREE_POS, TREE_SIZE, 0, tree_, true);
	}
	if (txt_ > 6 && txt_ < 8)
	{
		DrawOval(LIGHT_LX, DOWN_LIGHT, DOWN_SHADOW_X, DOWN_SHADOW_Y, gray, true);
		DrawOval(LIGHT_LX, UP_LIGHT, UP_SHADOW_X, UP_SHADOW_Y, shadow, true);
		DrawRotaGraph(LIGHT_LX, WATER_POS, WATER_SIZE, 0, water_, true);
	}
	if (txt_ > 8 && txt_ < 13)
	{
		DrawOval(LIGHT_RX, DOWN_LIGHT, DOWN_SHADOW_X, DOWN_SHADOW_Y, gray, true);
		DrawOval(LIGHT_RX, UP_LIGHT, UP_SHADOW_X, UP_SHADOW_Y, shadow, true);
		DrawRotaGraph(LIGHT_RX, PLAYER_POS, PLAYER_SIZE, 0, player_, true);
	}
	DrawRotaGraph(FRAME_X, FRAME_Y, FRAME_SIZE, 0, waku_, true);
}

void DemoScene::Release(void)
{
	// フォントサイズと種類をデフォルトに戻す
	SetFontSize(16); // ゲーム内の標準サイズに合わせて変更
	ChangeFont("ＭＳ ゴシック"); // またはゲーム全体で使う基本フォントに

	SoundManager::GetInstance().Stop(SoundManager::SRC::DEMO_BGM);
	SoundManager::GetInstance().Stop(SoundManager::SRC::SET_SE);
}