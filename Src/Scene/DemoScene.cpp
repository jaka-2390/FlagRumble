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
	god_[0] = LoadGraph("Data/Image/Demo/God/god.png");
	god_[1] = LoadGraph("Data/Image/Demo/God/goda.png");
	god_[2] = LoadGraph("Data/Image/Demo/God/godb.png");
	god_[3] = LoadGraph("Data/Image/Demo/God/godc.png");
	god_[4] = LoadGraph("Data/Image/Demo/God/godd.png");
	tri_ = LoadGraph("Data/Image/Demo/tri.png");
	gNo_ = 0;
	txt_ = -2;
	cnt_ = 0;

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::DEMO_BGM, Sound::TIMES::LOOP);
}
void DemoScene::Update(void)
{
	InputManager& ins = InputManager::GetInstance();

	old_ = now_;
	now_ = CheckHitKey(KEY_INPUT_RETURN);

	if (ins.IsTrgDown(KEY_INPUT_RETURN)) {
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
	if(txt_>= 1)gNo_=1;
	if(txt_>= 2)gNo_=0;
	if(txt_>= 4)gNo_=4;
	if(txt_>= 5)gNo_=0;
	if(txt_>= 7)gNo_=2;
	if(txt_>= 8)gNo_=0;
	if(txt_>= 9)gNo_=4;
	if(txt_>=10)gNo_=0;
	if(txt_>=13)gNo_=3;
	if(txt_>=14)gNo_=0;
	if(txt_>=15)gNo_=2;
	if(txt_>=16)gNo_=0;
#pragma endregion
}
void DemoScene::Draw(void)
{
	cnt_++;
	DrawBox(0,0,Application::SCREEN_SIZE_X,Application::SCREEN_SIZE_Y,0x0,true);
#pragma region テキスト
	DrawString(385,900+83*-1-83*txt_,"使徒諸君おはよう　神だ。",					0xffffff);
	DrawString(385,900+83*0 -83*txt_,"私を知っている？何を寝ぼけている。",			0xffffff);
	DrawString(385,900+83*1 -83*txt_,"お前は生まれ堕ちたばかりだろう。",			0xffffff);
	DrawString(385,900+83*2 -83*txt_,"まぁいい うまれてすぐだろうが神託だ。",		0xffffff);
	DrawString(385,900+83*3 -83*txt_,"お前にはある使命を与える。",					0xffffff);
	DrawString(385,900+83*4 -83*txt_,"これから生まれる世界の礎となる植物",			0xffffff);
	DrawString(385,900+83*5 -83*txt_,"「ユグドラシル」の守護と育成だ。",			0xffffff);
	DrawString(385,900+83*6 -83*txt_,"植物の生命力を狙うモンスターを倒し、",		0xffffff);
	DrawString(385,900+83*7 -83*txt_,"その生命力を奪って植物に注ぎ込め。",			0xffffff);
	DrawString(385,900+83*8 -83*txt_,"戦い方がわからない？",						0xffffff);
	DrawString(385,900+83*9 -83*txt_,"お前には私の力の一部を渡してある。",			0xffffff);
	DrawString(385,900+83*10-83*txt_,"矢印キーで周囲を見渡して敵を見つけて",		0xffffff);
	DrawString(385,900+83*11-83*txt_,"WASDキーで近づいてEキーで攻撃しろ。",			0xffffff);
	DrawString(385,900+83*12-83*txt_,"左Shiftを使えば少しは早く移動できる。",		0xffffff);
	DrawString(385,900+83*13-83*txt_,"あぁ　それと、",								0xffffff);
	DrawString(385,900+83*14-83*txt_,"敵の攻撃で倒れても私の加護で蘇らせる。",		0xffffff);
	DrawString(385,900+83*15-83*txt_,"だから何があっても成し遂げろ。",				0xffffff);
	DrawString(385,900+83*16-83*txt_,"神の使徒たるお前に失敗は許されない。",		0xffffff);
	DrawString(385,900+83*17-83*txt_,"しくじれば神罰だ。",							0xffffff);
	DrawString(385,900+83*18-83*txt_,"頼んだぞ。",									0xffffff);
#pragma endregion
	DrawBox(0,0,Application::SCREEN_SIZE_X,800,0x0,true);
	DrawBox(0,980,Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,0x0,true);

	if (cnt_ % 90 <= 45)DrawRotaGraph(1490, 985, 1.2, 0, tri_, true);
	else(DrawRotaGraph(1490, 975, 1.2, 0, tri_, true));

	// 三角形の中心座標（god_の描画位置と合わせる）
	int centerX = Application::SCREEN_SIZE_X / 2;
	int centerY = Application::SCREEN_SIZE_Y / 2 - 200;

	// 半径（辺の長さではなく、中心から頂点までの距離）
	float radius = 315.0f;

	// 頂点座標を計算（正三角形）
	int x1 = centerX + static_cast<int>(radius * cosf(DX_PI * 0.0f / 180.0f));
	int y1 = centerY + static_cast<int>(radius * sinf(DX_PI * 0.0f / 180.0f));
	int x2 = centerX + static_cast<int>(radius * cosf(DX_PI * 120.0f / 180.0f));
	int y2 = centerY + static_cast<int>(radius * sinf(DX_PI * 120.0f / 180.0f));
	int x3 = centerX + static_cast<int>(radius * cosf(DX_PI * 240.0f / 180.0f));
	int y3 = centerY + static_cast<int>(radius * sinf(DX_PI * 240.0f / 180.0f));

	DrawRotaGraph(Application::SCREEN_SIZE_X/2,Application::SCREEN_SIZE_Y/2-200,1,0,god_[gNo_],true);

	if (txt_>3&&txt_<6)
	{
		DrawOval(1300,700,150,40,0xaaaaaa,true);
		DrawOval(1300,695,90,30,0x888888,true);
		DrawRotaGraph(1300, 630, 0.35, 0, tree_, true);
	}
	if (txt_>6&&txt_<8)
	{
		DrawOval(500, 700, 150, 40, 0xaaaaaa, true);
		DrawOval(500, 695, 90, 30, 0x888888, true);
		DrawRotaGraph(500, 655, 7.0f, 0.0f, water_, true);
	}
	if (txt_>8&&txt_<13)
	{
		DrawOval(1300, 700, 150, 40, 0xaaaaaa, true);
		DrawOval(1300, 695, 90, 30, 0x888888, true);
		DrawRotaGraph(1300, 650, 3.5f, 0.0f, player_, true);
	}
	DrawRotaGraph(Application::SCREEN_SIZE_X/2,Application::SCREEN_SIZE_Y-175,0.8,0, waku_, true);
}

void DemoScene::Release(void)
{
	// フォントサイズと種類をデフォルトに戻す
	SetFontSize(16); // ゲーム内の標準サイズに合わせて変更
	ChangeFont("ＭＳ ゴシック"); // またはゲーム全体で使う基本フォントに

	SoundManager::GetInstance().Stop(SoundManager::SRC::DEMO_BGM);
	SoundManager::GetInstance().Stop(SoundManager::SRC::SET_SE);
}
