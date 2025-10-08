#include <algorithm>
#include <cmath>
#include "../Manager/ResourceManager.h"
#include "../Object/EnemyBase.h"
#include "DxLib.h"
#include "MiniMap.h"

MiniMap::MiniMap(float worldSize, int screenSize, int mapPosX, int mapPosY)
    : mapPixelSize(screenSize), mapPosX(mapPosX), mapPosY(mapPosY), imgMapTree_()
{
    worldHalfSize = worldSize / HALF_DIVISOR_F;
    scale = static_cast<float>(mapPixelSize) / worldSize;
}

void MiniMap::Init(void)
{
    //画像読み込み
    imgMapTree_ = LoadGraph("Data/Image/MapTree.png");
}

void MiniMap::Draw(const MapVector2& playerPos, float playerAngleRad,
    float cameraAngleRad,
    const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
    DrawBackground();
    DrawEnemies(enemies);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_HALF_TRANSPARENT);   //透過ON
    DrawCameraViewCone(playerPos, cameraAngleRad);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, ALPHA_OPAQUE);           //透過OFF
    DrawPlayer(playerPos, playerAngleRad);
}

void MiniMap::DrawBackground()
{
    //ミニマップの中心
    int centerX = mapPosX + mapPixelSize / HALF_DIVISOR;
    int centerY = mapPosY + mapPixelSize / HALF_DIVISOR;
    int RADIUS = mapPixelSize / HALF_DIVISOR;

    int outerRadius = mapPixelSize / HALF_DIVISOR;
    int borderWidth = BORDER_WIDTH;     //外枠の太さ
    int innerRadius = outerRadius - borderWidth;

    //外側を黒で塗る
    DrawCircle(centerX, centerY, outerRadius, BLACK, TRUE);

    //中身を緑で塗る
    DrawCircle(centerX, centerY, innerRadius, GREEN, TRUE);

    //Tree画像
    DrawRotaGraph(centerX, centerY, MAP_TREE_SCALE, 0.0f, imgMapTree_, true);

}

void MiniMap::DrawCameraViewCone(const MapVector2& playerPos, float cameraAngleRad)
{
    //ミニマップ座標に変換
    MapVector2 mapPos = WorldToMapPos(playerPos);
    int cx = static_cast<int>(mapPos.x);
    int cz = static_cast<int>(mapPos.z);

    if (!IsInsideCircle(cx, cz)) return;

    //カメラの方向(視界の中心方向)
    float angle = cameraAngleRad - DX_PI / HALF_DIVISOR_F;

    //視界の長さ
    float viewLength = CAMERA_VIEW_LENGTH;

    //視界の開き角
    float fovHalfAngle = CAMERA_FOV_HALF_ANGLE;

    //左右の端を計算
    float leftX = cx + std::cos(angle + fovHalfAngle) * viewLength;
    float leftY = cz + std::sin(angle + fovHalfAngle) * viewLength;

    float rightX = cx + std::cos(angle - fovHalfAngle) * viewLength;
    float rightY = cz + std::sin(angle - fovHalfAngle) * viewLength;

    //三角形を描画
    DrawTriangleAA(
        cx, cz,
        static_cast<int>(leftX), static_cast<int>(leftY),
        static_cast<int>(rightX), static_cast<int>(rightY),
        WHITE, TRUE);   //半透明の白
}

void MiniMap::DrawPlayer(const MapVector2& playerPos, float playerAngleRad)
{
    MapVector2 playerMapPos = WorldToMapPos(playerPos);
    int px = static_cast<int>(playerMapPos.x);
    int pz = static_cast<int>(playerMapPos.z);

    //ミニマップ内に収まるように制限
    if (!IsInsideCircle(px, pz)) return;

    //向きを示す三角形
    //向きの角度
    float angle = playerAngleRad - DX_PI / HALF_DIVISOR_F;

    //三角形のサイズ
    float size = PLAYER_ARROW_SIZE;

    //先端（進行方向）
    float frontX = px + std::cos(angle) * size;
    float frontY = pz + std::sin(angle) * size;

    //左右の角（進行方向と逆）
    float sideOffset = PLAYER_ARROW_SIDE_OFFSET;
    float leftX = px + std::cos(angle + sideOffset) * size;
    float leftY = pz + std::sin(angle + sideOffset) * size;
    float rightX = px + std::cos(angle - sideOffset) * size;
    float rightY = pz + std::sin(angle - sideOffset) * size;

    //プレイヤーの三角形を描画
    DrawTriangle(
        static_cast<int>(frontX), static_cast<int>(frontY),
        static_cast<int>(leftX), static_cast<int>(leftY),
        static_cast<int>(rightX), static_cast<int>(rightY),
        BLUE, TRUE);
}

void MiniMap::DrawEnemies(const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
    for (const auto& enemy : enemies)
    {
        auto pos = enemy->GetCollisionPos();

        //VECTORからMapVector2へ変換
        MapVector2 worldPos{ pos.x, pos.z };

        MapVector2 enemyMapPos = WorldToMapPos(worldPos);
        int ex = static_cast<int>(enemyMapPos.x);
        int ez = static_cast<int>(enemyMapPos.z);

        //ミニマップ内に収まるように制限
        if (!IsInsideCircle(ex, ez)) continue;

        //通常の敵の円の半径
        int RADIUS = ENEMY_RADIUS;

        //BOSSのときは円を大きく
        if (enemy->GetEnemyType() == EnemyBase::TYPE::BOSS)
        {
            RADIUS = ENEMY_BOSS_RADIUS;
        }

        //敵
        DrawCircle(ex, ez, RADIUS, RED, TRUE);
    }
}

bool MiniMap::IsInsideCircle(int x, int y) const
{
    int centerX = mapPosX + mapPixelSize / HALF_DIVISOR;
    int centerY = mapPosY + mapPixelSize / HALF_DIVISOR;
    int RADIUS = mapPixelSize / HALF_DIVISOR;

    int dx = x - centerX;
    int dy = y - centerY;

    return dx * dx + dy * dy <= RADIUS * RADIUS;
}

MapVector2 MiniMap::WorldToMapPos(const MapVector2& worldPos) const
{
    MapVector2 result;
    result.x = static_cast<int>((worldPos.x + worldHalfSize) * scale) + mapPosX;
    result.z = static_cast<int>((-worldPos.z + worldHalfSize) * scale) + mapPosY;

    return result;
}