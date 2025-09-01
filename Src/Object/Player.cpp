#include <string>
#include <vector>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Tree.h"
#include "Player.h"

//担当髙野

Player::Player(void)
{
	animationController_ = nullptr;
	enemy_ = nullptr;
	tree_ = nullptr;

	state_ = STATE::NONE;

	//足煙エフェクト
	effectSmokeResId_ = -1;
	effectSmokePleyId_ = -1;


	jumpPow_ = AsoUtility::VECTOR_ZERO;
	// 衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	//ジャンプの初期化
	isJump_ = false;
	imgShadow_ = -1;
	stepJump_ = 0.0f; //初期化しなかったら遷移時にジャンプを押してる間ジャンプし続ける

	//スピードアップ用のフラグ
	speedUpFlag_ = false;
	speedUpCnt_ = 1200;

	//攻撃の初期化
	normalAttack_ = 2;
	slashAttack_ = 1;
	exrAttack_ = 2;
	powerUpFlag_ = false;
	isAttack_ = false;
	isAttack2_ = false;
	exAttack_ = false;
	exTimer_ = 10000;
	lastExTime_ = -exTimer_;
	powerUpCnt_ = 1200;

	//ステ関連
	hp_ = HP;
	water_ = 0;

	// 無敵状態
	invincible_ = false;

	// アイコン
	imgPowerIcon_ = -1;
	imgSpeedIcon_ = -1;
	imgRotateAttackIcon_ = -1;

	// 移動が可能かどうか
	canMove_ = true;
	// 所持上限かどうか
	isMax_ = false;

	// 状態管理
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	// モデルの基本設定
	transform_.SetModel(resMng_.Load(
		ResourceManager::SRC::PLAYER).handleId_);
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = { 300.0f, 0.0f, 0.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	// 丸影画像
	imgShadow_ = resMng_.Load(
		ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	// アイコン画像
	imgPowerIcon_ = resMng_.Load(ResourceManager::SRC::POWER_UP_ICON).handleId_;
	imgSpeedIcon_ = resMng_.Load(ResourceManager::SRC::SPEED_UP_ICON).handleId_;
	imgRotateAttackIcon_ = resMng_.Load(ResourceManager::SRC::ROTA_ATTACK_ICON).handleId_;

	//足煙エフェクト
	effectSmokeResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::FOOT_SMOKE).handleId_;
	
	//パワーアップエフェクト
	effectPowerResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::EFF_POWER).handleId_;
	
	//スピードアップエフェクト
	effectSpeedResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::EFF_SPEED).handleId_;
	
	//回復エフェクト
	effectHealResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::EFF_HEAL).handleId_;

	// アニメーションの設定
	InitAnimation();


	// カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	collisionRadius_ = 100.0f;	// 衝突判定用の球体半径
	collisionLocalPos_ = { 0.0f, capsule_->GetCenter().y, 0.0f};	// 衝突判定用の球体中心の調整座標

	// 初期状態
	ChangeState(STATE::PLAY);
}

void Player::Update(void)
{
	// 更新ステップ
	stateUpdate_();

	transform_.Update();

	// アニメーション再生
	animationController_->Update();

	UpdateDown(1.0f);

	auto& ins = InputManager::GetInstance();
	if (ins.IsNew(KEY_INPUT_B))
	{
		powerUpFlag_ = true;
	}

	if (ins.IsNew(KEY_INPUT_N))
	{
		speedUpFlag_ = true;
	}

}

void Player::UpdateDown(float deltaTime)
{
	auto& ins = InputManager::GetInstance();
	//if (ins.IsNew(KEY_INPUT_I)) wHit(0.2f);

	if (pstate_ == PlayerState::DOWN) {
		isAttack_ = false;
		isAttack2_ = false;
		exAttack_ = false;
		revivalTimer_ += deltaTime;
		if (revivalTimer_ >= D_COUNT) {
			Revival();
		}
		return;
	}
}

void Player::Draw(void)
{
	MV1DrawModel(transform_.modelId);	// モデルの描画
	DrawShadow();						// 丸影描画
	//DrawDebug();						// デバッグ用描画

#pragma region ステータス
	DrawFormatString(55, Application::SCREEN_SIZE_Y - 95, 0x0, "PLAYER");
	// 枠線（白）
	DrawBox(47, Application::SCREEN_SIZE_Y - 78, 653 , Application::SCREEN_SIZE_Y - 37, 0xaaaaaa, true);
	
	DrawBox(50, Application::SCREEN_SIZE_Y - 75, 650, Application::SCREEN_SIZE_Y - 55, 0x0, true);
	if (hp_ != 0)DrawBox(50, Application::SCREEN_SIZE_Y - 75, hp_ * 60 + 50, Application::SCREEN_SIZE_Y - 55, 0x00ff00, true);
	if (hp_ == 0)DrawBox(50, Application::SCREEN_SIZE_Y - 75, revivalTimer_ + 50, Application::SCREEN_SIZE_Y - 55, 0xff0000, true);
	DrawBox(50, Application::SCREEN_SIZE_Y - 50, 650, Application::SCREEN_SIZE_Y - 40, 0x0, true);
	DrawBox(50, Application::SCREEN_SIZE_Y - 50, water_ * 60 + 50, Application::SCREEN_SIZE_Y - 40, 0x0000ff, true);
	
	if (powerUpFlag_)
	{
		const int cx = 150;
		const int iconCy = Application::SCREEN_SIZE_Y - 115;
		const int timerCy = iconCy + 2;  // タイマーだけ2px下げる
		const float radius = 32.0f;
		const int segments = 60;

		// アイコン描画
		DrawRotaGraph(cx, iconCy, 1.3, 0, imgPowerIcon_, true);

		// タイマー描画（黒い円グラフ）
		float ratio = static_cast<float>(powerUpCnt_) / POWER_UP_TIME;
		int filledSegments = static_cast<int>(segments * ratio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		for (int i = filledSegments; i < segments; ++i)
		{
			float angle1 = -DX_PI_F / 2 - DX_TWO_PI * i / segments;
			float angle2 = -DX_PI_F / 2 - DX_TWO_PI * (i + 1) / segments;

			float x1 = cx + radius * cosf(angle1);
			float y1 = timerCy + radius * sinf(angle1);  // timerCy使用
			float x2 = cx + radius * cosf(angle2);
			float y2 = timerCy + radius * sinf(angle2);  // timerCy使用

			DrawTriangle(cx, timerCy, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (speedUpFlag_)
	{
		const int cx = 225;
		const int iconCy = Application::SCREEN_SIZE_Y - 115;
		const int timerCy = iconCy + 2;  // タイマーだけ2px下げる
		const float radius = 32.0f;
		const int segments = 60;

		// アイコン描画
		DrawRotaGraph(cx, iconCy, 1.3, 0, imgSpeedIcon_, true);

		// タイマー描画（黒い円グラフ）
		float ratio = static_cast<float>(speedUpCnt_) / SPEED_UP_TIME;
		int filledSegments = static_cast<int>(segments * ratio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		for (int i = filledSegments; i < segments; ++i)
		{
			float angle1 = -DX_PI_F / 2 - DX_TWO_PI * i / segments;
			float angle2 = -DX_PI_F / 2 - DX_TWO_PI * (i + 1) / segments;

			float x1 = cx + radius * cosf(angle1);
			float y1 = timerCy + radius * sinf(angle1);  // timerCy使用
			float x2 = cx + radius * cosf(angle2);
			float y2 = timerCy + radius * sinf(angle2);  // timerCy使用

			DrawTriangle(cx, timerCy, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (tree_ && tree_->GetLv() >= 50) {
		const int cx = 450;
		const int iconCy = Application::SCREEN_SIZE_Y - 115;
		const int timerCy = iconCy + 2;
		const float radius = 32.0f;
		const int segments = 60;

		DrawRotaGraph(cx, iconCy, 1.3, 0, imgRotateAttackIcon_, true);

		if (!IsExAttackReady())
		{
			float ratio = static_cast<float>(GetNowCount() - lastExTime_) / exTimer_;
			int filledSegments = static_cast<int>(segments * ratio);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

			for (int i = 0; i < segments - filledSegments; ++i)
			{
				float angle1 = -DX_PI_F / 2 - DX_TWO_PI * i / segments;
				float angle2 = -DX_PI_F / 2 - DX_TWO_PI * (i + 1) / segments;

				float x1 = cx + radius * cosf(angle1);
				float y1 = timerCy + radius * sinf(angle1);
				float x2 = cx + radius * cosf(angle2);
				float y2 = timerCy + radius * sinf(angle2);

				DrawTriangle(cx, timerCy, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
			}

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

#pragma endregion
}

void Player::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void Player::ClearCollider(void)
{
	colliders_.clear();
}

void Player::SetEnemy(const std::vector<std::shared_ptr<EnemyBase>>* enemys)
{
	enemy_ = enemys;
}

VECTOR Player::GetPos() const
{
	return transform_.pos;
}

void Player::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

const Capsule& Player::GetCapsule(void) const
{
	return *capsule_;
}

VECTOR Player::GetCollisionPos(void) const
{
	return VAdd(collisionLocalPos_, transform_.pos);
}

float Player::GetCollisionRadius(void)
{
	return collisionRadius_;
}

const std::vector<std::shared_ptr<EnemyBase>>& Player::GetEnemyCollision(void) const
{
	return *enemy_;
}

bool Player::IsPlay(void) const
{
	return state_ == STATE::PLAY;
}

void Player::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "NPlayer/";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Player.mv1", 60.0f, 1);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Player.mv1", 17.0f,2);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "Player.mv1", 13.0f, 3);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Player.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK2, path + "Player.mv1", 17.0f, 4);
	animationController_->Add((int)ANIM_TYPE::ATTACK1, path + "Player.mv1", 17.0f, 5);
	animationController_->Add((int)ANIM_TYPE::DOWN, path + "Player.mv1", 15.0f, 7);
	animationController_->Add((int)ANIM_TYPE::EXATTACK, path + "Player.mv1", 15.0f, 8);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{

	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();

}

void Player::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&Player::UpdatePlay, this);
}

void Player::UpdatePlay(void)
{
	if (!canMove_)return;
	//スピードアップの制限時間
	SpeedUpTimer();

	//移動処理
	ProcessMove();

	// 移動方向に応じた回転
	Rotate();

	// ジャンプ処理
	ProcessJump();

	//パワーアップの制限時間
	PowerUpTimer();

	// 攻撃処理
	ProcessAttack();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//移動
	transform_.pos = movedPos_;

	// 重力方向に沿って回転させる
	transform_.quaRot = grvMng_.GetTransform().quaRot;
	transform_.quaRot = transform_.quaRot.Mult(playerRotY_);

	// 歩きエフェクト
	EffectFootSmoke();

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectPowerPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
	SetPosPlayingEffekseer3DEffect(effectSpeedPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
	SetPosPlayingEffekseer3DEffect(effectHealPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::DrawShadow(void)
{
	int i, j;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{
		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			c.lock()->modelId_,
			-1,
			transform_.pos,
			VAdd(transform_.pos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

void Player::DrawDebug(void)
{

	int white = 0xffffff;
	int black = 0x000000;
	int red = 0xff0000;
	int green = 0x00ff00;
	int blue = 0x0000ff;
	int yellow = 0xffff00;
	int purpl = 0x800080;

	/*VECTOR v;

	// キャラ基本情報
	//-------------------------------------------------------
	// キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 60, white, "Player座標 ： (%0.2f, %0.2f, %0.2f)%d", v.x, v.y, v.z, hp_);
	//-------------------------------------------------------

	// 衝突
	DrawLine3D(gravHitPosUp_, gravHitPosDown_, 0x000000);*/

	
	/*if (isAttack_) {

		VECTOR forward = transform_.quaRot.GetForward();
		VECTOR attackCollisionPos = VAdd(transform_.pos, VScale(forward, 100.0f));
		float attackCollisionRadius = 100.0f;
		// カプセルの描画確認用	
		DrawSphere3D(attackCollisionPos, attackCollisionRadius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), FALSE);
	}if (isAttack2_) {

		VECTOR forward = transform_.quaRot.GetForward();
		VECTOR attackCollisionPos = VAdd(transform_.pos, VScale(forward, 80.0f));
		attackCollisionPos.y += 100.0f;
		float attackCollisionRadius = 140.0f;
		// カプセルの描画確認用	
		DrawSphere3D(attackCollisionPos, attackCollisionRadius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), FALSE);
	}
	if (exAttack_) {
		VECTOR attackCollisionPos = transform_.pos;
		attackCollisionPos.y += 100.0f;
		float attackCollisionRadius = 180.0f;
		// カプセルの描画確認用	
		DrawSphere3D(attackCollisionPos, attackCollisionRadius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), FALSE);
	}*/

	//capsule_->Draw();

	/*VECTOR s;
	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, red, red, false);*/
}

void Player::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	//方向量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = mainCamera->GetQuaRotOutX();

	//方向
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	double rotRad = 0;

	if (!isAttack_ && !isAttack2_ && !exAttack_ && IsEndLandingA())
	{
		if (ins.IsNew(KEY_INPUT_W))
		{
			dir = cameraRot.GetForward();
			rotRad = AsoUtility::Deg2RadF(0.0f);
		}
		if (ins.IsNew(KEY_INPUT_S))
		{
			dir = cameraRot.GetBack();
			rotRad = AsoUtility::Deg2RadF(180.0f);
		}
		if (ins.IsNew(KEY_INPUT_D))
		{
			dir = cameraRot.GetRight();
			rotRad = AsoUtility::Deg2RadF(90.0f);
		}
		if (ins.IsNew(KEY_INPUT_A))
		{
			dir = cameraRot.GetLeft();
			rotRad = AsoUtility::Deg2RadF(-90.0f);
		}

		if ((!AsoUtility::EqualsVZero(dir)) &&
			(isJump_ || IsEndLanding()))
		{
			//移動量
			speed_ = SPEED_MOVE;
			if (ins.IsNew(KEY_INPUT_LSHIFT))
			{
				speed_ = SPEED_RUN;
			}
			
			//アイテム獲得時のスピード
			if (speedUpFlag_)
			{
				speed_ = speed_ * STATUS_UP;
			}

			moveDir_ = dir;
			//移動量
			movePow_ = VScale(dir, speed_);

			// 回転処理IDLE
			SetGoalRotate(rotRad);

			if ((!isJump_ && IsEndLanding()))
			{
				// アニメーション
				if (ins.IsNew(KEY_INPUT_LSHIFT))
				{
					animationController_->Play((int)ANIM_TYPE::FAST_RUN);
				}
				else
				{
					animationController_->Play((int)ANIM_TYPE::RUN);
				}
			}
		}
		else
		{
			if ((!isJump_ && IsEndLanding()))
			{
				animationController_->Play((int)ANIM_TYPE::IDLE);
			}
		}
	}
}

void Player::SetGoalRotate(double rotRad)
{

	VECTOR cameraRot = mainCamera->GetAngles();

	Quaternion axis =
		Quaternion::AngleAxis(
			(double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);

}

void Player::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// 移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	collisionPos_ = VAdd(transform_.pos, collisionLocalPos_);
}

void Player::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	// 重力方向の反対
	VECTOR dirUpGravity = grvMng_.GetDirUpGravity();

	// 重力の強さ
	float gravityPow = grvMng_.GetPower();

	float checkPow = 10.0f;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));

	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));

	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//if (hit.HitFlag > 0)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{
			// 衝突地点から、少し上に移動

			// 地面と衝突している
			// 押し戻し処理とジャンプ力の打ち消しを実装しましょう

			//movedPos_に押し戻し座標を設定
			//押し戻し座標については、dxlib のhit構造体の中にヒントアリ
			//衝突地点情報が格納されている

			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			//jumpPow_の値をゼロにする
			//ジャンプのリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			stepJump_ = 0.0f;
			if (isJump_)
			{
				// 着地モーション
				animationController_->Play(
					(int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
			}
			isJump_ = false;

		}
	}
}

void Player::CollisionCapsule(void)
{
	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);
	// カプセルとの衝突判定
	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		// 衝突した複数のポリゴンと衝突回避するまで、
		// プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
			// この時、移動させる方向は、移動前座標に向いた方向であったり、
			// 衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// 法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					// カプセルも一緒に移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void Player::CollisionAttack(void)
{
	if (isAttack_ || enemy_)
	{
		//エネミーとの衝突判定
		
		// 攻撃の球の半径
		float attackRadius = 100.0f;
		// 攻撃の方向(プレイヤーの前方)
		VECTOR forward = transform_.quaRot.GetForward();
		// 攻撃の開始位置と終了位置
		VECTOR attackPos = VAdd(transform_.pos, VScale(forward, 100.0f));

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			// 球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos,attackRadius,enemyPos,enemyRadius))
			{
				enemy->Damage(normalAttack_);
				// 1体のみヒット
				break;
			}
		}
	}
}

void Player::CollisionAttack2(void)
{
	if (isAttack2_ || enemy_)
	{
		//エネミーとの衝突判定

		// 攻撃の球の半径
		float attackRadius = 140.0f;
		// 攻撃の方向(プレイヤーの前方)
		VECTOR forward = transform_.quaRot.GetForward();
		// 攻撃の開始位置と終了位置
		VECTOR attackPos = VAdd(transform_.pos, VScale(forward, 80.0f));
		attackPos.y += 100.0f;  // 攻撃の高さ調整

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			// 球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos, attackRadius, enemyPos, enemyRadius))
			{
				enemy->Damage(slashAttack_);
				// 複数ヒット
				continue;
			}
		}
	}
}

void Player::CollisionAttackEx(void)
{
	if (exAttack_ || enemy_)
	{
		//エネミーとの衝突判定

		// 攻撃の球の半径
		float attackRadius = 180.0f;
		// 攻撃の開始位置と終了位置
		VECTOR attackPos = transform_.pos;
		attackPos.y += 100.0f;  // 攻撃の高さ調整

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			// 球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos, attackRadius, enemyPos, enemyRadius))
			{
				enemy->Damage(exrAttack_);
				// 複数ヒットさせたいなら
				continue;
			}
		}
	}
}

void Player::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	// 重力の強さ
	float gravityPow = grvMng_.GetPower();

	// 重力
	// 重力を作る
	// メンバ変数 jumpPow_ に重力計算を行う(加速度)
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

void Player::ProcessJump(void)
{

	bool isHit = CheckHitKey(KEY_INPUT_BACK);

	// ジャンプ
	if (isHit && !isAttack_ && !isAttack2_ && !exAttack_ && (isJump_ || IsEndLanding()))
	{
		if (!isJump_)
		{
			// 制御無しジャンプ
			//animationController_->Play((int)ANIM_TYPE::JUMP);

			// この後、いくつかのジャンプパターンを試します
			//無理やりアニメーション
			animationController_->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
			animationController_->SetEndLoop(23.0f, 25.0f, 5.0f);
		}

		isJump_ = true;

		// ジャンプの入力受付時間をヘラス
		stepJump_ += scnMng_.GetDeltaTime();

		if (stepJump_ < TIME_JUMP_IN)
		{
			jumpPow_ = VScale(grvMng_.GetDirUpGravity(), POW_JUMP);
		}

	}

	// ボタンを離したらジャンプ力に加算しない
	if (!isHit)
	{
		stepJump_ = TIME_JUMP_IN;
	}

}

bool Player::IsEndLanding(void)
{
	bool ret = true;

	// アニメーションがジャンプではない
	if (animationController_->GetPlayType() != (int)ANIM_TYPE::JUMP)
	{
		return ret;
	}
	//アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}
	return false;

}

void Player::ProcessAttack(void)
{
	bool isHit = CheckHitKey(KEY_INPUT_E);
	bool isHit_N = CheckHitKey(KEY_INPUT_Q);
	bool isHit_E = CheckHitKey(KEY_INPUT_R);

	// アタック
	if (!isJump_ && (isAttack_ || IsEndLandingA()))
	{
		if (!isAttack_ && !isAttack2_ && !exAttack_ && isHit)
		{
			animationController_->Play((int)ANIM_TYPE::ATTACK1, false);
			isAttack_ = true;

			// 衝突(攻撃)
			CollisionAttack();
			
			// 攻撃音①
			SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE1, Sound::TIMES::FORCE_ONCE);

		}
		else if (!isAttack2_ && !isAttack_ && !exAttack_ && isHit_N)
		{
			// Treeのレベルが25以上ならATTACK2を許可
			if (tree_ && tree_->GetLv() >= 25 && !isAttack2_)
			{
				animationController_->Play((int)ANIM_TYPE::ATTACK2, false);
				isAttack2_ = true;

				// 衝突(攻撃)
				CollisionAttack2();

				// 攻撃音②
				SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE2, Sound::TIMES::FORCE_ONCE);
			}
		}
		else if (!exAttack_ && !isAttack_ && !isAttack2_ && isHit_E)
		{
			// Treeのレベルが50以上でクールタイム10秒がたっているならEXATTACKを許可
			if (tree_ && tree_->GetLv() >= 50 && !exAttack_ && IsExAttackReady())
			{
				animationController_->Play((int)ANIM_TYPE::EXATTACK, false);
				exAttack_ = true;
				lastExTime_ = GetNowCount(); // ← クールタイム開始

				// 衝突(攻撃)
				CollisionAttackEx();

				// 攻撃音③
				SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE3, Sound::TIMES::FORCE_ONCE);
			}
		}
	}

	// アニメーションが終わったらフラグをリセット
	if (animationController_->IsEnd())
	{
		if (isAttack_)
		{
			isAttack_ = false;
		}
		if (isAttack2_)
		{
			isAttack2_ = false;
		}
		if (exAttack_)
		{
			exAttack_ = false;
		}
	}
}

bool Player::IsEndLandingA(void)
{
	bool ret = true;
	int animType = animationController_->GetPlayType();

	// 現在のアニメーションが ATTACK1,2 または EXATTACK のいずれかで、まだ終了していない場合
	if ((animType != (int)ANIM_TYPE::ATTACK1 || animType == (int)ANIM_TYPE::ATTACK2 
		|| animType == (int)ANIM_TYPE::EXATTACK) 
		&& !animationController_->IsEnd())
	{
		return ret;
	}
	// アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}
	return false;
}

bool Player::IsExAttackReady() const
{
	int now = GetNowCount(); // DxLib の現在時刻（ミリ秒）
	return (now - lastExTime_) >= exTimer_;
}

void Player::Damage(int damage)
{
	if (pstate_ == PlayerState::DOWN || invincible_) return;  // ダウン中は無敵
	hp_ -= damage;

	// SE
	SoundManager::GetInstance().Play(SoundManager::SRC::P_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);

	if (hp_ <= 0) {
		hp_ = 0;

		// SE
		SoundManager::GetInstance().Play(SoundManager::SRC::P_DOWN_SE, Sound::TIMES::ONCE);
		StartRevival();  // 死亡ではなく復活待機
	}
}

void Player::PowerUpTimer(void)
{
	//攻撃アップ
	if (powerUpFlag_)
	{
		powerUpCnt_--;

		if (powerUpCnt_ <= 0)
		{
			powerUpFlag_ = false;

			normalAttack_ = 2;
			slashAttack_ = 1;
			exrAttack_ = 2;
			powerUpCnt_ = POWER_UP_TIME;
		}
	}
}

void Player::PowerUp(void)
{
	powerUpFlag_ = true;

	EffectPower();

	// パワーアップ
	SoundManager::GetInstance().Play(SoundManager::SRC::POWERUP_SE, Sound::TIMES::ONCE);

	if (powerUpCnt_ >= 0 && powerUpFlag_)
	{
		normalAttack_ = normalAttack_ * STATUS_UP;
		slashAttack_ = slashAttack_ * STATUS_UP;
		exrAttack_ = exrAttack_ * STATUS_UP;
	}
}

void Player::SpeedUpTimer(void)
{
	//攻撃アップ
	if (speedUpFlag_)
	{
		speedUpCnt_--;

		if (speedUpCnt_ <= 0)
		{
			speedUpFlag_ = false;
			speedUpCnt_ = SPEED_UP_TIME;
		}
	}
}

void Player::SpeedUp(void)
{
	speedUpFlag_ = true;

	EffectSpeed();

	// スピードアップ
	SoundManager::GetInstance().Play(SoundManager::SRC::SPEEDUP_SE, Sound::TIMES::ONCE);
}

void Player::Heal(void)
{
	hp_ = HP;

	// 回復
	SoundManager::GetInstance().Play(SoundManager::SRC::HEAL_SE, Sound::TIMES::ONCE);
	EffectHeal();
}

void Player::Muteki(void)
{
	invincible_ = true;
}

void Player::StartRevival()
{
	invincible_ = true;   // 無敵状態にする
	canMove_ = false;     // 移動停止

	pstate_ = PlayerState::DOWN;
	revivalTimer_ = 0.0f;

	animationController_->Play((int)ANIM_TYPE::DOWN, false);
	// 必要なら移動や入力を停止させる
}

void Player::Revival()
{
	hp_ = HP;
	pstate_ = PlayerState::NORMAL;

	// 復活後の無敵状態を解除
	invincible_ = false;   // 無敵解除
	// プレイヤーが移動可能になる
	canMove_ = true;   // 移動再開

	animationController_->Play((int)ANIM_TYPE::IDLE, true);
	// 他の再開処理（無敵終了、移動可能など）をここで
}

void Player::EffectFootSmoke(void)
{
	float len = AsoUtility::MagnitudeF(moveDiff_);

	stepFootSmoke_ -= scnMng_.GetDeltaTime();

	//if (!AsoUtility::EqualsVZero(moveDiff_))
	if (stepFootSmoke_ < 0.0f && len >= 1.0f && !isJump_)
	{
		stepFootSmoke_ = TERM_FOOT_SMOKE;

		// エフェクト再生
		effectSmokePleyId_ = PlayEffekseer3DEffect(effectSmokeResId_);

		//エフェクトの大きさ
		SetScalePlayingEffekseer3DEffect(effectSmokePleyId_, 5.0f, 5.0f, 5.0f);

		//エフェクトの位置
		SetPosPlayingEffekseer3DEffect(effectSmokePleyId_,transform_.pos.x, transform_.pos.y, transform_.pos.z);
	}
}

void Player::EffectPower(void)
{
	float scale = 20.0f;

	// エフェクト再生
	effectPowerPleyId_ = PlayEffekseer3DEffect(effectPowerResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectPowerPleyId_, scale, scale, scale);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectPowerPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::EffectSpeed(void)
{
	float scale = 20.0f;

	// エフェクト再生
	effectSpeedPleyId_ = PlayEffekseer3DEffect(effectSpeedResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectSpeedPleyId_, scale, scale, scale);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectSpeedPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::EffectHeal(void)
{
	float scale = 20.0f;

	// エフェクト再生
	effectHealPleyId_ = PlayEffekseer3DEffect(effectHealResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectHealPleyId_, scale, scale, scale);

}

int Player::GetWater(void) const
{
	return water_;
}

bool Player::IsMax(void)
{
	return isMax_;
}
void Player::SetIsMax(void)
{
	isMax_ = false;
}

void Player::SetTree(Tree* tree)
{
	tree_ = tree;
}

void Player::wHit(float scale)
{

	// SE
	SoundManager::GetInstance().Play(SoundManager::SRC::GETWATER_SE, Sound::TIMES::FORCE_ONCE);

	// 増加量
	int add = 1;

	// スケールに応じて増加量を変える
	if (scale >= 0.2f) {
		add = 3;
	}
	else if (scale >= 0.15f) {
		add = 2;
	}
	// それ未満は1
	water_+= add;
	if (water_ > WATER_MAX)water_ = WATER_MAX;
}
void Player::tHit()
{
	if (water_ == WATER_MAX)
	{
		isMax_ = true;
		water_ = 0;
	}
	else
	{
		water_--;
	}
}