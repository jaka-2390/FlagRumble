#include <string>
#include <vector>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Player.h"

Player::Player(void)
{
	animationController_ = nullptr;
	enemy_ = nullptr;

	state_ = STATE::NONE;

	//足煙エフェクト
	effectSmokeResId_ = -1;
	effectSmokePleyId_ = -1;

	//衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	//丸影
	imgShadow_ = -1;

	//スピードアップ用のフラグ
	speedUpFlag_ = false;
	speedUpCnt_ = SPEED_UP_TIME;

	//攻撃の初期化
	normalAttack_ = NORMAL_ATTACK;
	slashAttack_ = SLASH_ATTACK;
	exrAttack_ = EX_ATTACK;
	powerUpFlag_ = false;
	isAttack_ = false;
	isAttack2_ = false;
	exAttack_ = false;
	exTimer_ = EX_TIME;
	lastExTime_ = -exTimer_;
	powerUpCnt_ = POWER_UP_TIME;

	//ステ関連
	hp_ = HP;
	water_ = 0;

	//無敵状態
	invincible_ = false;

	//アイコン
	imgPowerIcon_ = -1;
	imgSpeedIcon_ = -1;
	imgRotateAttackIcon_ = -1;

	//移動が可能かどうか
	canMove_ = true;
	//所持上限かどうか
	isMax_ = false;

	//状態管理
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	//モデルの基本設定
	transform_.SetModel(resMng_.Load(
		ResourceManager::SRC::PLAYER).handleId_);
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = PLAYER_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(PLAYER_ROT_Y), 0.0f });
	transform_.Update();

	//丸影画像
	imgShadow_ = resMng_.Load(
		ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	//アイコン画像
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
	
	//攻撃エフェクト
	effectSwordResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::SWORD).handleId_;

	//アニメーションの設定
	InitAnimation();

	//カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop(CAPSULE_TOP);
	capsule_->SetLocalPosDown(CAPSULE_BOTTOM);
	capsule_->SetRadius(CAPSULE_RADIUS);

	//衝突判定用の球体半径
	collisionRadius_ = COLLISION_RADIUS;

	//衝突判定用の球体中心の調整座標
	collisionLocalPos_ = { 0.0f, capsule_->GetCenter().y - COLL_OFFSET, 0.0f };

	//初期状態
	ChangeState(STATE::PLAY);
}

void Player::Update(void)
{
	//更新ステップ
	stateUpdate_();

	//移動方向への回転
	auto moveRot = Quaternion::LookRotation(moveDir_);

	transform_.quaRot = Quaternion::Slerp(
		transform_.quaRot, moveRot, 0.2f
	);

	//モデル制御更新
	transform_.Update();

	//アニメーション再生
	animationController_->Update();

	//ダウン処理
	UpdateDown(DOWN_DELTATIME);
}

void Player::UpdateDown(float deltaTime)
{
	auto& ins = InputManager::GetInstance();

	if (pstate_ == PlayerState::DOWN && animationController_->IsEnd())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
	}

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
	MV1DrawModel(transform_.modelId);	//モデルの描画
	DrawShadow();						//丸影描画
	//DrawDebug();						//デバッグ用描画

#pragma region ステータス
	DrawFormatString(NAME_X, NAME_Y, black, "PLAYER");
	//枠線
	DrawBox(FRAME_START_X, FRAME_START_Y, FRAME_END_X, FRAME_END_Y, gray, true);

	DrawBox(BAR_START_X, BAR_START_HY, BAR_END_X, BAR_END_HY, black, true);
	if (hp_ != 0)DrawBox(BAR_START_X, BAR_START_HY, hp_ * BAR_POINT + BAR_START_X, BAR_END_HY, green, true);
	

	if (powerUpFlag_)
	{
		//アイコン描画
		DrawRotaGraph(POWER_CX, ICON_CY, ICON_SIZE, 0, imgPowerIcon_, true);

		//タイマー描画（黒い円グラフ）
		float ratio = static_cast<float>(powerUpCnt_) / POWER_UP_TIME;
		int filledSegments = static_cast<int>(SEGMENTS * ratio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, GRAY_ALPHA);
		for (int i = filledSegments; i < SEGMENTS; ++i)
		{
			float angle1 = -(static_cast<float>(DX_PI_F) / HALF_DIVISOR) - static_cast<float>(DX_TWO_PI) * i / SEGMENTS;
			float angle2 = -(static_cast<float>(DX_PI_F) / HALF_DIVISOR) - static_cast<float>(DX_TWO_PI) * (i + 1) / SEGMENTS;

			float x1 = POWER_CX + RADIUS * cosf(angle1);
			float y1 = TIMER_CY + RADIUS * sinf(angle1);  //timerCy使用
			float x2 = POWER_CX + RADIUS * cosf(angle2);
			float y2 = TIMER_CY + RADIUS * sinf(angle2);  //timerCy使用

			DrawTriangle(POWER_CX, TIMER_CY, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (speedUpFlag_)
	{
		//アイコン描画
		DrawRotaGraph(SPEED_CX, ICON_CY, ICON_SIZE, 0, imgSpeedIcon_, true);

		//タイマー描画（黒い円グラフ）
		float ratio = static_cast<float>(speedUpCnt_) / SPEED_UP_TIME;
		int filledSegments = static_cast<int>(SEGMENTS * ratio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, GRAY_ALPHA);
		for (int i = filledSegments; i < SEGMENTS; ++i)
		{
			float angle1 = -(static_cast<float>(DX_PI_F) / HALF_DIVISOR) - static_cast<float>(DX_TWO_PI) * i / SEGMENTS;
			float angle2 = -(static_cast<float>(DX_PI_F) / HALF_DIVISOR) - static_cast<float>(DX_TWO_PI) * (i + 1) / SEGMENTS;

			float x1 = SPEED_CX + RADIUS * cosf(angle1);
			float y1 = TIMER_CY + RADIUS * sinf(angle1);  //timerCy使用
			float x2 = SPEED_CX + RADIUS * cosf(angle2);
			float y2 = TIMER_CY + RADIUS * sinf(angle2);  //timerCy使用

			DrawTriangle(SPEED_CX, TIMER_CY, (int)x1, (int)y1, (int)x2, (int)y2, GetColor(0, 0, 0), true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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

void Player::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Player/";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Player.mv1", IDLE_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Player.mv1", ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "Player.mv1", ANIM_SPEED, ANIM_FAST_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::SLASHATTACK, path + "Player.mv1", ANIM_SPEED, ANIM_SLASHATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::NORMALATTACK, path + "Player.mv1", ANIM_SPEED, ANIM_NORMALATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DAMAGE, path + "Player.mv1", ANIM_SPEED, ANIM_DAMAGE_INDEX);
	animationController_->Add((int)ANIM_TYPE::DOWN, path + "Player.mv1", ANIM_SPEED, ANIM_DOWN_INDEX);
	animationController_->Add((int)ANIM_TYPE::EXATTACK, path + "Player.mv1", ANIM_SPEED, ANIM_EXATTACK_INDEX);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

void Player::ChangeState(STATE state)
{
	//状態変更
	state_ = state;

	//各状態遷移の初期処理
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

	//移動方向に応じた回転
	Rotate();

	//パワーアップの制限時間
	PowerUpTimer();

	//攻撃処理
	ProcessAttack();

	//重力による移動量
	CalcGravityPow();

	//落下処理
	ProcessFall();

	//衝突判定
	Collision();

	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//移動
	transform_.pos = movedPos_;

	//重力方向に沿って回転させる
	/*transform_.quaRot = grvMng_.GetTransform().quaRot;
	transform_.quaRot = transform_.quaRot.Mult(playerRotY_);*/

	//歩きエフェクト
	EffectFootSmoke();

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectPowerPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
	SetPosPlayingEffekseer3DEffect(effectSpeedPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
	SetPosPlayingEffekseer3DEffect(effectHealPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::DrawShadow(void)
{
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	//ライティングを無効にする
	SetUseLighting(FALSE);

	//Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	//テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{
		//地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			c.lock()->modelId_,
			-1,
			transform_.pos,
			VAdd(transform_.pos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);

		//頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(SHADOW_COLLER, SHADOW_COLLER, SHADOW_COLLER, SHADOW_COLLER);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		//球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			//ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			//ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, SHADOW_LIFT);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			//ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			for (int i = 0; i < 3; i++)
			{
				float diff = fabs(HitRes->Position[i].y - transform_.pos.y);

				if (HitRes->Position[i].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				{
					float alpha = static_cast<float>(SHADOW_MAX_ALPHA) *
						(1.0f - diff / PLAYER_SHADOW_HEIGHT);

					//0～255にクランプしてからBYTEへキャスト
					alpha = std::clamp(alpha, 0.0f, 255.0f);

					Vertex[i].dif.a = static_cast<unsigned char>(alpha + 0.5f);
				}
				else
				{
					Vertex[i].dif.a = 0;
				}
			}

			//ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;

			//影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		//検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	//ライティングを有効にする
	SetUseLighting(TRUE);

	//Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

void Player::DrawDebug(void)
{
#ifdef _DEBUG

	VECTOR v;

	//キャラ基本情報
	//-------------------------------------------------------
	//キャラ座標
	v = transform_.pos;
	DrawFormatString(0, 0, black, "Player座標 ： (%0.2f, %0.2f, %0.2f)%d", v.x, v.y, v.z, hp_);
	//-------------------------------------------------------

	//衝突
	DrawLine3D(gravHitPosUp_, gravHitPosDown_, black);

	capsule_->Draw();
#endif
}

void Player::ProcessMove(void)
{
	//方向量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = mainCamera->GetQuaRotOutX();

	//方向
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	double rotRad = 0;

	if (!isAttack_ && !isAttack2_ && !exAttack_ && IsEndLanding())
	{
		if (GetJoypadNum() == 0)
		{
			if (ins.IsNew(KEY_INPUT_W))
			{
				dir = cameraRot.GetForward();
				rotRad = AsoUtility::Deg2RadF(ROT_FORWARD_DEG);
			}
			if (ins.IsNew(KEY_INPUT_S))
			{
				dir = cameraRot.GetBack();
				rotRad = AsoUtility::Deg2RadF(ROT_BACK_DEG);
			}
			if (ins.IsNew(KEY_INPUT_D))
			{
				dir = cameraRot.GetRight();
				rotRad = AsoUtility::Deg2RadF(ROT_RIGHT_DEG);
			}
			if (ins.IsNew(KEY_INPUT_A))
			{
				dir = cameraRot.GetLeft();
				rotRad = AsoUtility::Deg2RadF(ROT_LEFT_DEG);
			}
		}
		else
		{
			// 接続されているゲームパッド１の情報を取得
			InputManager::JOYPAD_IN_STATE padState =
				ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

			// アナログキーの入力値から方向を取得
			dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

			//カメラがクォータニオンを採用している場合
			//アナログキーをカメラ方向に回転
			dir = cameraRot.PosAxis(dir);
		}

		if (!AsoUtility::EqualsVZero(dir))
		{
			//移動量
			speed_ = SPEED_MOVE;
			if (ins.IsNew(KEY_INPUT_LSHIFT) || 
				ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::RB))
			{
				speed_ = SPEED_RUN;

				//アニメーション
				animationController_->Play((int)ANIM_TYPE::FAST_RUN);
			}
			else
			{
				//アニメーション
				animationController_->Play((int)ANIM_TYPE::RUN);
			}

			//アイテム獲得時のスピード
			if (speedUpFlag_)
			{
				speed_ = speed_ * STATUS_UP;
			}

			moveDir_ = dir;
			//移動量
			movePow_ = VScale(dir, speed_);

			//回転処理IDLE
			SetGoalRotate(rotRad);
		}
		else
		{
			animationController_->Play((int)ANIM_TYPE::IDLE);
		}
	}
}

void Player::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot = mainCamera->GetAngles();

	Quaternion axis =
		Quaternion::AngleAxis(
			(double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	//現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	//しきい値
	if (angleDiff > SIKII)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	//回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Player::Collision(void)
{
	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//衝突(カプセル)
	CollisionCapsule();

	//衝突(重力)
	CollisionGravity();

	//移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	collisionPos_ = VAdd(transform_.pos, collisionLocalPos_);
}

void Player::CollisionGravity(void)
{
	//ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	//重力方向の反対
	VECTOR dirUpGravity = grvMng_.GetDirUpGravity();

	//重力の強さ
	float gravityPow = grvMng_.GetPower();

	float checkPow = 10.0f;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));

	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));

	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		//地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//if(hit.HitFlag > 0)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{
			//衝突地点から、少し上に移動
			//地面と衝突している
			//movedPos_に押し戻し座標を設定
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			//jumpPow_の値をゼロにする
			jumpPow_ = AsoUtility::VECTOR_ZERO;
		}
	}
}

void Player::CollisionCapsule(void)
{
	//カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);
	//カプセルとの衝突判定
	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		//衝突した複数のポリゴンと衝突回避するまで、
		//プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			//地面と異なり、衝突回避位置が不明なため、何度か移動させる
			//この時、移動させる方向は、移動前座標に向いた方向であったり、
			//衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < CAPSULE_CNT; tryCnt++)
			{
				//再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				//最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					//法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					//カプセルも一緒に移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		//検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void Player::CollisionAttack(void)
{
	if (isAttack_ || enemy_)
	{
		//エネミーとの衝突判定

		//攻撃の球の半径
		float attackRadius = ATTACK_RADIUS;
		//攻撃の方向(プレイヤーの前方)
		VECTOR forward = transform_.quaRot.GetForward();
		//攻撃の開始位置と終了位置
		VECTOR attackPos = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD));

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			//球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos, attackRadius, enemyPos, enemyRadius))
			{
				EffectSword();

				SetPosPlayingEffekseer3DEffect(effectSwordPleyId_, attackPos.x, attackPos.y + ATTACK2_HEIGHT, attackPos.z);

				enemy->Damage(normalAttack_);
				//複数ヒット
				continue;
			}
		}
	}
}

void Player::CollisionAttack2(void)
{
	if (isAttack2_ || enemy_)
	{
		//エネミーとの衝突判定

		//攻撃の球の半径
		float attackRadius = ATTACK2_RADIUS;
		//攻撃の方向(プレイヤーの前方)
		VECTOR forward = transform_.quaRot.GetForward();
		//攻撃の開始位置と終了位置
		VECTOR attackPos = VAdd(transform_.pos, VScale(forward, ATTACK2_FORWARD));
		attackPos.y += ATTACK2_HEIGHT;  //攻撃の高さ調整

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			//球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos, attackRadius, enemyPos, enemyRadius))
			{
				enemy->Damage(slashAttack_);
				//複数ヒット
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

		//攻撃の球の半径
		float attackRadius = EX_RADIUS;
		//攻撃の開始位置と終了位置
		VECTOR attackPos = transform_.pos;
		attackPos.y += EX_HEIGHT;  //攻撃の高さ調整

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			//球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackPos, attackRadius, enemyPos, enemyRadius))
			{
				enemy->Damage(exrAttack_);
				//複数ヒットさせたいなら
				continue;
			}
		}
	}
}

void Player::CalcGravityPow(void)
{
	//重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	//重力の強さ
	float gravityPow = grvMng_.GetPower();

	//重力を作る
	//メンバ変数 jumpPow_ に重力計算を行う(加速度)
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	//内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		//重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

void Player::ProcessFall(void)
{
	if (transform_.pos.y <= -300.0f)
	{
		transform_.pos = PLAYER_POS;
		Damage(3);
	}
}

void Player::ProcessAttack(void)
{
	bool isHit = CheckHitKey(KEY_INPUT_E) ||
		ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::TOP);
	bool isHit_N = CheckHitKey(KEY_INPUT_Q);
	bool isHit_E = CheckHitKey(KEY_INPUT_R);

	//アタック
	if (isAttack_ || IsEndLanding())
	{
		if (!isAttack_ && !isAttack2_ && !exAttack_ && isHit)
		{
			animationController_->Play((int)ANIM_TYPE::NORMALATTACK, false);
			isAttack_ = true;
			
			//衝突(攻撃)
			CollisionAttack();

			//攻撃音①
			SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE1, Sound::TIMES::FORCE_ONCE);

		}
		else if (!isAttack2_ && !isAttack_ && !exAttack_ && isHit_N)
		{
			animationController_->Play((int)ANIM_TYPE::SLASHATTACK, false);
			isAttack2_ = true;

			//衝突(攻撃)
			CollisionAttack2();

			//攻撃音②
			SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE2, Sound::TIMES::FORCE_ONCE);
		}
		else if (!exAttack_ && !isAttack_ && !isAttack2_ && isHit_E)
		{
			animationController_->Play((int)ANIM_TYPE::EXATTACK, false);
			exAttack_ = true;
			lastExTime_ = GetNowCount(); //← クールタイム開始

			//衝突(攻撃)
			CollisionAttackEx();

			//攻撃音③
			SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE3, Sound::TIMES::FORCE_ONCE);
		}
	}

	//アニメーションが終わったらフラグをリセット
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

bool Player::IsEndLanding(void)
{
	bool ret = true;
	int animType = animationController_->GetPlayType();

	//現在のアニメーションが ATTACK1,2 または EXATTACK のいずれかで、まだ終了していない場合
	if (animType != (int)ANIM_TYPE::NORMALATTACK)
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

bool Player::IsExAttackReady() const
{
	int now = GetNowCount(); //DxLib の現在時刻（ミリ秒）
	return (now - lastExTime_) >= exTimer_;
}

void Player::Damage(int damage)
{
	if (pstate_ == PlayerState::DOWN || invincible_) return;  //ダウン中は無敵
	hp_ -= damage;

	//アニメーション
	//animationController_->Play((int)ANIM_TYPE::DAMAGE, false);

	//SE
	SoundManager::GetInstance().Play(SoundManager::SRC::P_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);

	if (hp_ <= 0) {
		hp_ = 0;

		//SE
		SoundManager::GetInstance().Play(SoundManager::SRC::P_DOWN_SE, Sound::TIMES::ONCE);
		StartRevival();  //死亡ではなく復活待機
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

			normalAttack_ = NORMAL_ATTACK;
			slashAttack_ = SLASH_ATTACK;
			exrAttack_ = EX_ATTACK;
			powerUpCnt_ = POWER_UP_TIME;
		}
	}
}

void Player::PowerUp(void)
{
	powerUpFlag_ = true;

	EffectPower();

	//パワーアップ
	SoundManager::GetInstance().Play(SoundManager::SRC::POWERUP_SE, Sound::TIMES::ONCE);

	if (powerUpCnt_ >= 0 && powerUpFlag_)
	{
		normalAttack_ = normalAttack_ * static_cast<int>(STATUS_UP);
		slashAttack_ = slashAttack_ * static_cast<int>(STATUS_UP);
		exrAttack_ = exrAttack_ * static_cast<int>(STATUS_UP);
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

	//スピードアップ
	SoundManager::GetInstance().Play(SoundManager::SRC::SPEEDUP_SE, Sound::TIMES::ONCE);
}

void Player::Heal(void)
{
	hp_ = HP;

	//回復
	SoundManager::GetInstance().Play(SoundManager::SRC::HEAL_SE, Sound::TIMES::ONCE);
	EffectHeal();
}

void Player::Muteki(void)
{
	invincible_ = true;
}

void Player::StartRevival()
{
	invincible_ = true;   //無敵状態にする
	canMove_ = false;     //移動停止

	pstate_ = PlayerState::DOWN;
	revivalTimer_ = 0.0f;

	animationController_->Play((int)ANIM_TYPE::DOWN, false);
}

void Player::Revival()
{
	hp_ = HP;
	pstate_ = PlayerState::NORMAL;

	//復活後の無敵状態を解除
	invincible_ = false;   //無敵解除
	//プレイヤーが移動可能になる
	canMove_ = true;   //移動再開

	animationController_->Play((int)ANIM_TYPE::IDLE, true);
}

void Player::EffectFootSmoke(void)
{
	float len = AsoUtility::MagnitudeF(moveDiff_);

	stepFootSmoke_ -= scnMng_.GetDeltaTime();

	if (stepFootSmoke_ < 0.0f && len >= 1.0f)
	{
		stepFootSmoke_ = TERM_FOOT_SMOKE;

		//エフェクト再生
		effectSmokePleyId_ = PlayEffekseer3DEffect(effectSmokeResId_);

		//エフェクトの大きさ
		SetScalePlayingEffekseer3DEffect(effectSmokePleyId_, FOOT_SMOKE_SCALE, FOOT_SMOKE_SCALE, FOOT_SMOKE_SCALE);

		//エフェクトの位置
		SetPosPlayingEffekseer3DEffect(effectSmokePleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
	}
}

void Player::EffectPower(void)
{
	float scale = STATUS_EFFECT_SCALE;

	//エフェクト再生
	effectPowerPleyId_ = PlayEffekseer3DEffect(effectPowerResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectPowerPleyId_, scale, scale, scale);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectPowerPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::EffectSpeed(void)
{
	float scale = STATUS_EFFECT_SCALE;

	//エフェクト再生
	effectSpeedPleyId_ = PlayEffekseer3DEffect(effectSpeedResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectSpeedPleyId_, scale, scale, scale);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectSpeedPleyId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void Player::EffectHeal(void)
{
	float scale = STATUS_EFFECT_SCALE;

	//エフェクト再生
	effectHealPleyId_ = PlayEffekseer3DEffect(effectHealResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectHealPleyId_, scale, scale, scale);
}

void Player::EffectSword(void)
{
	float scale = STATUS_EFFECT_SCALE;

	//エフェクト再生
	effectSwordPleyId_ = PlayEffekseer3DEffect(effectSwordResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectSwordPleyId_, scale, scale, scale);
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

void Player::wHit(float scale)
{
	//SE
	SoundManager::GetInstance().Play(SoundManager::SRC::GETWATER_SE, Sound::TIMES::FORCE_ONCE);

	//増加量
	int add = WATER_SMALL;

	//スケールに応じて増加量を変える
	if (scale >= WATER_SCALE_BIG) {
		add = WATER_BIG;
	}
	else if (scale >= WATER_SCALE_MID) {
		add = WATER_MID;
	}
	//それ未満は1
	water_ += add;
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