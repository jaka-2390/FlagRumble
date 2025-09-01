#include "MiniMap.h"
#include "DxLib.h"
#include "../Manager/ResourceManager.h"
#include "../Object/EnemyBase.h"
#include <algorithm> // std::clamp
#include <cmath>     // std::atan2, std::cos, std::sin

MiniMap::MiniMap(float worldSize, int screenSize, int mapPosX, int mapPosY)
    : mapPixelSize(screenSize), mapPosX(mapPosX), mapPosY(mapPosY),imgMapTree_()
{
    worldHalfSize = worldSize / HALF_DIVISOR_F;
    scale = static_cast<float>(mapPixelSize) / worldSize;
}

void MiniMap::Init(void)
{
    // 画像読み込み
    imgMapTree_ = LoadGraph("Data/Image/MapTree.png");
}

void MiniMap::Draw(const MapVector2& playerPos, float playerAngleRad,
    float cameraAngleRad,
    const std::vector<std::shared_ptr<EnemyBase>>& enemies,
    const std::vector<MapVector2>& items)
{
    DrawBackground();
    DrawEnemies(enemies);
    DrawItems(items);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_HALF_TRANSPARENT);      //透過ON
    DrawCameraViewCone(playerPos, cameraAngleRad);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, ALPHA_OPAQUE);      //透過OFF
    DrawPlayer(playerPos, playerAngleRad);
}

void MiniMap::DrawBackground()
{
    // === ミニマップの中心 ===
    int centerX = mapPosX + mapPixelSize / HALF_DIVISOR;
    int centerY = mapPosY + mapPixelSize / HALF_DIVISOR;
    int radius = mapPixelSize / HALF_DIVISOR;

    int outerRadius = mapPixelSize / HALF_DIVISOR;
    int borderWidth = BORDER_WIDTH; // 外枠の太さ
    int innerRadius = outerRadius - borderWidth;

    // --- 外側を緑で塗る(枠線を含む)
    DrawCircle(centerX, centerY, outerRadius, BLACK, TRUE);

    // --- 上から中身だけを「円でくり抜く」ことでリング状になる
    DrawCircle(centerX, centerY, innerRadius, GREEN, TRUE);

    // 画像
    DrawRotaGraph(centerX, centerY, MAP_TREE_SCALE, 0.0f, imgMapTree_, true);

}

void MiniMap::DrawCameraViewCone(const MapVector2& playerPos, float cameraAngleRad)
{
    // ミニマップ座標に変換
    MapVector2 mapPos = WorldToMapPos(playerPos);
    int px = static_cast<int>(mapPos.x);
    int pz = static_cast<int>(mapPos.z);

    if (!IsInsideCircle(px, pz)) return;

    // カメラの方向(視界の中心方向)
    float angle = cameraAngleRad - DX_PI / HALF_DIVISOR_F;

    // 視界の長さ(半径に相当)
    float viewLength = CAMERA_VIEW_LENGTH;

    // 視界の開き角(左右にどれだけ開くか)
    float fovHalfAngle = CAMERA_FOV_HALF_ANGLE; // 約57度くらい(0.5ラジアン)

    // 左・右方向の端を計算(底辺の左右端)
    float leftX = px + std::cos(angle + fovHalfAngle) * viewLength;
    float leftY = pz + std::sin(angle + fovHalfAngle) * viewLength;

    float rightX = px + std::cos(angle - fovHalfAngle) * viewLength;
    float rightY = pz + std::sin(angle - fovHalfAngle) * viewLength;

    // 三角形を描画（カメラの視界：半透明）
    DrawTriangleAA(
        px, pz,
        static_cast<int>(leftX), static_cast<int>(leftY),
        static_cast<int>(rightX), static_cast<int>(rightY),
        WHITE, TRUE); // 半透明の白
}

void MiniMap::DrawPlayer(const MapVector2& playerPos, float playerAngleRad)
{
    // +worldHalfSize で中心原点を左上基準にずらす
    // * scale で画面上のスケールに変換
    // + offsetX / Y で画面上に配置
    MapVector2 playerMapPos = WorldToMapPos(playerPos);
    int px = static_cast<int>(playerMapPos.x);
    int pz = static_cast<int>(playerMapPos.z);

    // === ミニマップ内に収まるように制限===
    if (!IsInsideCircle(px, pz)) return;

    // === 向きを示す三角形（矢印） ===
    // 向きの角度（ラジアン）
    float angle = playerAngleRad - DX_PI / HALF_DIVISOR_F; // 適宜調整してね

    // 矢印のサイズ
    float size = PLAYER_ARROW_SIZE;

    // 先端（進行方向）
    float frontX = px + std::cos(angle) * size;
    float frontY = pz + std::sin(angle) * size;

    // 左右の角（扇形に広げた方向）
    float sideOffset = PLAYER_ARROW_SIDE_OFFSET; // 三角の広がり角度
    float leftX = px + std::cos(angle + sideOffset) * size;
    float leftY = pz + std::sin(angle + sideOffset) * size;
    float rightX = px + std::cos(angle - sideOffset) * size;
    float rightY = pz + std::sin(angle - sideOffset) * size;

    // プレイヤーの三角形を描画
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
        auto pos = enemy->GetCollisionPos();  // 仮に座標取得関数があるとする

        // VECTORからMapVector2へ変換
        MapVector2 worldPos{ pos.x, pos.z };

        // +worldHalfSize で中心原点を左上基準にずらす
        // * scale で画面上のスケールに変換
        // + offsetX / Y で画面上に配置
        MapVector2 enemyMapPos = WorldToMapPos(worldPos);
        int ex = static_cast<int>(enemyMapPos.x);
        int ez = static_cast<int>(enemyMapPos.z);

        // === ミニマップ内に収まるように制限===
        if (!IsInsideCircle(ex, ez)) continue;

        // 通常の円の半径
        int radius = ENEMY_RADIUS;

        // BOSSのときは円を大きく
        if (enemy->GetEnemyType() == EnemyBase::TYPE::BOSS)
        {
            radius = ENEMY_BOSS_RADIUS;
        }

        // 敵(赤)
        DrawCircle(ex, ez, radius, RED, TRUE);
    }
}

void MiniMap::DrawItems(const std::vector<MapVector2>& items)
{
    for (const auto& item : items)
    {
        MapVector2 mapPos = WorldToMapPos(item);
        int ix = static_cast<int>(mapPos.x);
        int iz = static_cast<int>(mapPos.z);

        // === ミニマップ内に収まるように制限===
        if (!IsInsideCircle(ix, iz)) continue;

        int size = ITEM_SIZE;
        // 頂点座標
        int topX = ix;
        int topY = iz - size;
        int rightX = ix + size;
        int rightY = iz;
        int bottomX = ix;
        int bottomY = iz + size;
        int leftX = ix - size;
        int leftY = iz;

        // 三角形を2つ使って塗りつぶす
        DrawTriangle(topX, topY, rightX, rightY, bottomX, bottomY, LIGHT_BLUE, TRUE);
        DrawTriangle(bottomX, bottomY, leftX, leftY, topX, topY, LIGHT_BLUE, TRUE);

    }
}

bool MiniMap::IsInsideCircle(int x, int y) const
{
    int centerX = mapPosX + mapPixelSize / HALF_DIVISOR;
    int centerY = mapPosY + mapPixelSize / HALF_DIVISOR;
    int radius = mapPixelSize / HALF_DIVISOR;

    int dx = x - centerX;
    int dy = y - centerY;

    return dx * dx + dy * dy <= radius * radius;
}

MapVector2 MiniMap::WorldToMapPos(const MapVector2& worldPos) const
{
    MapVector2 result;
    result.x = static_cast<int>((worldPos.x + worldHalfSize) * scale) + mapPosX;
    result.z = static_cast<int>((-worldPos.z + worldHalfSize) * scale) + mapPosY;
    return result;
}
