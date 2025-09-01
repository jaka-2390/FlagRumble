#include<DxLib.h>
#include<EffekseerForDXLib.h>
#include"../Common/Vector2.h"
#include"../Scene/GameScene.h"
#include"../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include"../Application.h"
#include"Player.h"
#include"Tree.h"

Tree::Tree(void)
{
	lv_ = ZERO;
	isAlive_ = false;
	hp_ = ZERO;
	water_ = ZERO;
	dir_ = {};
	modelIdB_ = ZERO;
	modelIdK_ = ZERO;
	modelIdA_ = ZERO;
	modelIdO_ = ZERO;

	// 無敵状態
	imgMutekiIcon_ = 0;
	invincible_ = false;
	mutekiCnt_ = 600;

	//エフェクト
	effectTreeResId_ = 0;
	effectTreePlayId_ = 0;
}
Tree::~Tree(void)
{
}

bool Tree::Init(void)
{
	//モデルの読込
	modelIdB_ = MV1LoadModel((Application::PATH_MODEL + "wood/Baby.mv1").c_str());
	modelIdK_ = MV1LoadModel((Application::PATH_MODEL + "wood/Kid_ver2.mv1").c_str());
	modelIdA_ = MV1LoadModel((Application::PATH_MODEL + "wood/Adult_ver2.mv1").c_str());
	modelIdO_ = MV1LoadModel((Application::PATH_MODEL + "wood/Old.mv1").c_str());

	//無敵アイコン画像
	imgMutekiIcon_ = LoadGraph("Data/Image/Icon/MUTEKIIcon.png");

	scl_ = { 3.0f, 2.5f, 3.0f };							//大きさ
	rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };			//回転
	pos_ = { 0.0f, -3.5f, 0.0f };							//位置

	lv_ = 1;
	isAlive_ = true;
	isD_ = false;
	grow_ = Tree::GROW::BABY;
	hp_ = 50;
	water_ = 0;

	//衝突判定
	collisionRadius_ = 100.0f;								
	collisionLocalPos_ = { 0.0f, 60.0f, 0.0f };				

	//エフェクト
	effectTreeResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::TREE_RANGE).handleId_;

	return true;
}
void Tree::Update(void)
{
	if (!player_) return;

	//プレイヤーとの距離をXZ平面だけで測る
	VECTOR playerPos = player_->GetPos();
	VECTOR treePos = pos_;

	float dx = playerPos.x - treePos.x;
	float dz = playerPos.z - treePos.z;
	float distance = sqrtf(dx * dx + dz * dz);

	//成長段階に応じた最小距離を設定
	float minDistance = 0.0f;
	switch (grow_) 
	{
	case GROW::BABY:
		minDistance = 70.0f;
		break;
	case GROW::KID:
		minDistance = 100.0f;
		collisionRadius_ = 200.0f;
		break;
	case GROW::ADULT:
		minDistance = 350.0f;
		collisionRadius_ = 350.0f;
		break;
	case GROW::OLD:
		minDistance = 700.0f;
		collisionRadius_ = 800.0f;
		break;
	}

	//プレイヤーが円の内側にいたら押し出す
	if (distance < minDistance) 
	{
		float len = sqrtf(dx * dx + dz * dz);
		if (len > 0.001f) 
		{
			dx /= len;
			dz /= len;

			VECTOR newPos = {
				treePos.x + dx * minDistance,
				playerPos.y,
				treePos.z + dz * minDistance};

			player_->SetPos(newPos);//←PlayerにSetPosが必要
		}
	}

	if (distance < viewRange_ && player_->GetWater() > 0)
	{
		player_->tHit();
		pHit();//プレイヤーが近くて水を持ってたら水を貯める
	}

	switch (grow_)
	{
	case Tree::GROW::BABY:
		scl_ = { 3.0f, 2.5f, 3.0f };						// 大きさの設定
		rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };		// 角度の設定
		pos_ = { 0.0f, -3.5f, 0.0f };						// 位置の設定
		MV1SetScale(modelIdB_, scl_);						//３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
		MV1SetRotationXYZ(modelIdB_, rot_);					//３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
		MV1SetPosition(modelIdB_, pos_);					//３Ｄモデルの位置(引数は、３Ｄ座標)
		break;
	case Tree::GROW::KID:
		rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };		// 角度の設定
		scl_ = { 15.0f, 10.0f, 15.0};
		pos_ = { 0.0f, -2.0f, 0.0f };
		MV1SetScale(modelIdK_, scl_);						//３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
		MV1SetRotationXYZ(modelIdK_, rot_);					//３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
		MV1SetPosition(modelIdK_, pos_);					//３Ｄモデルの位置(引数は、３Ｄ座標)
		break;
	case Tree::GROW::ADULT:
		rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };		// 角度の設定
		scl_ = { 30.0f, 25.0f, 30.0f };
		pos_ = { 0.0f, -2.5f, 0.0f };
		MV1SetScale(modelIdA_, scl_);						//３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
		MV1SetRotationXYZ(modelIdA_, rot_);					//３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
		MV1SetPosition(modelIdA_, pos_);					//３Ｄモデルの位置(引数は、３Ｄ座標)
		break;
	case Tree::GROW::OLD:
		rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };		// 角度の設定
		scl_ = { 45.0f, 45.0f, 45.0f };
		pos_ = { 0.0f, -23.5f, 0.0f };
		MV1SetScale(modelIdO_, scl_);						//３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
		MV1SetRotationXYZ(modelIdO_, rot_);					//３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
		MV1SetPosition(modelIdO_, pos_);					//３Ｄモデルの位置(引数は、３Ｄ座標)
		break;
	}

	collisionPos_ = VAdd(pos_, collisionLocalPos_);
	DrawDebugTree2Player();
	EffectTreeRange();	//エフェクト
	MutekiTimer();		//無敵時間

	auto& ins = InputManager::GetInstance();
}
void Tree::Draw(void)
{
	// モデルの描画
	switch (grow_)
	{
	case Tree::GROW::BABY:
		MV1SetScale(modelIdB_, scl_);
		MV1SetRotationXYZ(modelIdB_, rot_);
		MV1SetPosition(modelIdB_, pos_);
		MV1DrawModel(modelIdB_);
		break;
	case Tree::GROW::KID:
		MV1DrawModel(modelIdK_);
		break;
	case Tree::GROW::ADULT:
		MV1DrawModel(modelIdA_);
		break;
	case Tree::GROW::OLD:
		MV1DrawModel(modelIdO_);
		break;
	}

#pragma region ステータス
	DrawFormatString(55,Application::SCREEN_SIZE_Y-220,0x0,"YGGDRASILL : Lv%d",lv_);
	// 枠線（白）
	DrawBox(47, Application::SCREEN_SIZE_Y - 203, 653, Application::SCREEN_SIZE_Y - 162, 0xaaaaaa, true);

	DrawBox(50,Application::SCREEN_SIZE_Y-200,650,Application::SCREEN_SIZE_Y-180,0x0,true);
	if(isD_ == true)
	{
		 DrawBox(50,Application::SCREEN_SIZE_Y-200,hp_*12+50,Application::SCREEN_SIZE_Y-180,0xff0000,true);
		 isD_ = false;
	}
	else DrawBox(50,Application::SCREEN_SIZE_Y-200,hp_*12+50,Application::SCREEN_SIZE_Y-180,0x00ff00,true);

								DrawBox(50,Application::SCREEN_SIZE_Y-175,          650,Application::SCREEN_SIZE_Y-165,0x0,     true);
	     if(grow_==GROW::OLD)	DrawBox(50,Application::SCREEN_SIZE_Y-175,water_*200+50,Application::SCREEN_SIZE_Y-165,0x0000ff,true);
	else if(grow_==GROW::ADULT)	DrawBox(50,Application::SCREEN_SIZE_Y-175,water_*300+50,Application::SCREEN_SIZE_Y-165,0x0000ff,true);
	else if(grow_==GROW::KID)	DrawBox(50,Application::SCREEN_SIZE_Y-175,water_*300+50,Application::SCREEN_SIZE_Y-165,0x0000ff,true);
	else if(grow_==GROW::BABY)	DrawBox(50,Application::SCREEN_SIZE_Y-175,water_*600+50,Application::SCREEN_SIZE_Y-165,0x0000ff,true);

	if (invincible_)
	{
		const int cx = 300;
		const int cy = Application::SCREEN_SIZE_Y - 115;
		const float radius = 32.0f;
		const int segments = 60;

		// 無敵アイコン画像
		DrawRotaGraph(cx, cy, 1.3, 0.0, imgMutekiIcon_, true);

		float ratio = static_cast<float>(mutekiCnt_) / INVINCIBLE_TIME;
		int filledSegments = static_cast<int>(segments * ratio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

		for (int i = filledSegments; i < segments; ++i)
		{
			float angle1 = -DX_PI_F / 2 - DX_TWO_PI * i / segments;
			float angle2 = -DX_PI_F / 2 - DX_TWO_PI * (i + 1) / segments;

			float x1 = cx + radius * cosf(angle1);
			float y1 = cy + radius * sinf(angle1);
			float x2 = cx + radius * cosf(angle2);
			float y2 = cy + radius * sinf(angle2);

			DrawTriangle(cx, cy, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
#pragma endregion
}

void Tree::DrawDebug(void)
{
	int white = 0xffffff;
	int black = 0x000000;
	int red = 0xff0000;
	int green = 0x00ff00;
	int blue = 0x0000ff;
	int yellow = 0xffff00;
	int purpl = 0x800080;

	VECTOR v;
	VECTOR c;

	if (invincible_)
	{
		DrawFormatString(50, 180, GetColor(255, 0, 0), "���G: �c��%d�b", mutekiCnt_);
	}
}

void Tree::DrawDebugTree2Player(void)
{
	// プレイヤーとの距離判定して円を描く処理を追加
	if (player_ != nullptr)
	{
		VECTOR centerPos = pos_;  // 木の中心座標
		centerPos.y = 0.0f;		  // 円を下げる
		VECTOR playerPos = player_->GetTransform().pos;

		float dx = playerPos.x - centerPos.x;
		float dz = playerPos.z - centerPos.z;
		float distance = sqrtf(dx * dx + dz * dz);
		bool inRange = (distance <= viewRange_);

		unsigned int color = inRange ? 0xff0000 : 0xffdead;  /// 赤 or 薄黄色

		float angleStep = DX_PI_F * 2.0f / circleSegments_;

		if (grow_ == GROW::BABY) 
		{
			
		}
		else if (grow_ == GROW::KID)
		{
			centerPos.y = 0.0f;
			viewRange_ = 200.0f;
			
		}
		else if(grow_ == GROW::ADULT)
		{
			centerPos.y = 0.0f;
			viewRange_ = 450.0f;
			
		}
		else if(grow_ == GROW::OLD)
		{
			centerPos.y = 0.0f;
			viewRange_ = 909.0f;
			
		}

		for (int i = 0; i < circleSegments_; ++i)
		{
			float angle1 = angleStep * i;
			float angle2 = angleStep * (i + 1);

			VECTOR p1 = {
				centerPos.x + viewRange_ * sinf(angle1),
				centerPos.y,
				centerPos.z + viewRange_ * cosf(angle1)
			};

			VECTOR p2 = {
				centerPos.x + viewRange_ * sinf(angle2),
				centerPos.y,
				centerPos.z + viewRange_ * cosf(angle2)
			};

			DrawTriangle3D(centerPos, p1, p2, color, false);
		}
	}
}

int Tree::GetHp(void)
{
	return hp_;
}
int Tree::GetLv(void)
{
	return lv_;
}

void Tree::LvUp(void)
{
	if (grow_ == GROW::OLD && water_ >= 3)
	{
		lv_ += 1;
		water_ -= 3;
		ChangeGrow();
	}
	if (grow_ == GROW::ADULT && water_ >= 2)
	{
		lv_ += 1;
		water_ -= 2;
		ChangeGrow();
	}
	if (grow_ == GROW::KID && water_ >= 2)
	{
		lv_ += 1;
		water_ -= 2;
		ChangeGrow();
	}
	if (grow_ == GROW::BABY && water_ >= 1)
	{
		lv_ += 1;
		water_ -= 1;
		ChangeGrow();
	}

	// 水を与えたときの音
	SoundManager::GetInstance().Play(SoundManager::SRC::LEVEL_UP_SE, Sound::TIMES::ONCE);

}
void Tree::ChangeGrow(void)
{
	if (lv_ == 75)
	{
		grow_ = Tree::GROW::OLD;
		hp_ = 50;
		//PushEnemy();
	}
	else if (lv_ == 50)
	{
		grow_ = Tree::GROW::ADULT;
		hp_ = 50;
	}
	else if (lv_ == 25)
	{
		grow_ = Tree::GROW::KID;
		hp_ = 50;
	}
	else if (lv_ == 1)
	{
		grow_ = GROW::BABY;
		hp_ = 50;
	}
}

void Tree::SetPlayer(Player* player)
{
	player_ = player;
}

VECTOR Tree::GetCollisionPos(void) const
{
	return VAdd(collisionLocalPos_, pos_);
}

float Tree::GetCollisionRadius(void)
{
	return collisionRadius_;
}

void Tree::MutekiTimer(void)
{
	//無敵
	if (invincible_)
	{
		mutekiCnt_--;

		if (mutekiCnt_ <= 0)
		{
			invincible_ = false;
			mutekiCnt_ = 600;
		}
	}
}

void Tree::Muteki(void)
{
	invincible_ = true;

	// 木が無敵になった時の音
	SoundManager::GetInstance().Play(SoundManager::SRC::MUTEKI_SE, Sound::TIMES::ONCE);
}
void Tree::PushEnemy(void)
{
	collisionRadius_ += TREE_GROW_RADIUS_INCREMENT;
	if (!gameScene_) return;

	float pushOutRadius = collisionRadius_ + ENEMY_SAFE_MARGIN;

	for (const auto& enemyPtr : gameScene_->GetEnemies())
	{
		if (!enemyPtr) continue;

		VECTOR enemyPos = enemyPtr->GetTransform().pos;
		float dx = enemyPos.x - pos_.x;
		float dz = enemyPos.z - pos_.z;
		float distance = sqrtf(dx * dx + dz * dz);

		if (distance < pushOutRadius && distance > MIN_DISTANCE_THRESHOLD)
		{
			float scale = pushOutRadius / distance;
			enemyPtr->SetPos({
				pos_.x + dx * scale,
				enemyPos.y,
				pos_.z + dz * scale
				});
		}
	}
}
void Tree::eHit(void)//エネミーとのあたり判定
{
	if (!invincible_)
	{
		hp_ -= 1;
		isD_ = true;

		// 木がダメージを食らった音
		SoundManager::GetInstance().Play(SoundManager::SRC::T_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);
	}
}
void Tree::pHit(void)//プレイヤーとのあたり判定
{
	if (player_->IsMax() == true)
	{
		player_->SetIsMax();
		for (int l = 0; l <= 19; l++)
		{
			water_++;
			LvUp();
			ChangeGrow();
		}
	}
	else
	{
		water_++;
		LvUp();
		ChangeGrow();
	}
}


void Tree::EffectTreeRange(void)
{

	if (effectTreePlayId_ >= 0)
	{
		StopEffekseer3DEffect(effectTreePlayId_);
	}

	float scale = 10.0f;  // デフォルト値

	switch (grow_)
	{
	case GROW::BABY:
		scale = 50.0f;
		break;
	case GROW::KID:
		scale = 105.0f;
		break;
	case GROW::ADULT:
		scale = 235.0f;
		break;
	case GROW::OLD:
		scale = 480.0f;
		break;
	default:
		break;
	}

	effectTreePlayId_ = PlayEffekseer3DEffect(effectTreeResId_);
	SetScalePlayingEffekseer3DEffect(effectTreePlayId_, scale, scale, scale);
	SetPosPlayingEffekseer3DEffect(effectTreePlayId_, pos_.x, pos_.y, pos_.z);
}