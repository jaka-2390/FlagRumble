#pragma once
#include <memory>
#include <vector>

class EnemyBase;

struct MapVector2
{
    float x, z;
};

class MiniMap
{
public:
    // 定数

    // ミニマップのX座標
    static constexpr int DEFAULT_MAP_POS_X = 1600;
    // ミニマップのY座標
    static constexpr int DEFAULT_MAP_POS_Y = 20;

    // ÷ 2
    static constexpr int HALF_DIVISOR = 2;
    // ÷ 2.0f
    static constexpr float HALF_DIVISOR_F = 2.0f;

    // 外枠の太さ
    static constexpr int BORDER_WIDTH = 4;

    // 半透明ON
    static constexpr int ALPHA_HALF_TRANSPARENT = 128;
    // 不透明に戻す
    static constexpr int ALPHA_OPAQUE = 0;

    // プレイヤーのサイズ
    static constexpr int PLAYER_ARROW_SIZE = 8;
    // 三角の広がり角度
    static constexpr float PLAYER_ARROW_SIDE_OFFSET = 2.5f;

    // 敵の円の半径
    static constexpr int ENEMY_RADIUS = 3;
    // ボスの円の半径
    static constexpr int ENEMY_BOSS_RADIUS = 9;

    // アイテムのサイズ
    static constexpr int ITEM_SIZE = 5;

    // 木の画像サイズ
    static constexpr float MAP_TREE_SCALE = 0.15f;

    // 視界の長さ(半径に相当)
    static constexpr float CAMERA_VIEW_LENGTH = 30.0f;
    // 視界の開き角(左右にどれだけ開くか)
    static constexpr float CAMERA_FOV_HALF_ANGLE = 0.5f; // 約57度

    //黒(外枠)
    static constexpr int BLACK = 0x000000;
    //緑(マップ)
    static constexpr int GREEN = 0x64ff00;
    //白(視界)
    static constexpr int WHITE = 0xffffff;
    //青(プレイヤー)
    static constexpr int BLUE = 0x0000ff;
    //赤(敵)
    static constexpr int RED = 0xff0000;
    //水色(アイテム)
    static constexpr int LIGHT_BLUE = 0x0064ff;


    MiniMap(float worldSize, int screenSize, int mapPosX = DEFAULT_MAP_POS_X, int mapPosY = DEFAULT_MAP_POS_Y);

    void Init(void);

    void Draw(const MapVector2& playerPos, float playerAngleRad,
        float cameraAngleRad,
        const std::vector<std::shared_ptr<EnemyBase>>& enemies,
        const std::vector<MapVector2>& items);

private:
    float worldHalfSize;      // ゲーム空間の広さ（片側）
    int mapPixelSize;         // ミニマップの表示サイズ（ピクセル）
    int mapPosX, mapPosY;   // ミニマップの画面上の描画位置（左上）
    float scale;              // ワールド座標からミニマップ座標へのスケーリング係数

    // 画像
    int imgMapTree_;

    //Map背景描画
    void DrawBackground();
    //カメラの向きをミニマップ上に変換して描画
    void DrawCameraViewCone(const MapVector2& playerPos, float cameraAngleRad);
    //プレイヤーの位置をミニマップ上に変換して描画
    void DrawPlayer(const MapVector2& playerPos, float playerAngleRad);
    //敵をリストで受け取り、ミニマップ上に変換して描画
    void DrawEnemies(const std::vector<std::shared_ptr<EnemyBase>>& enemies);
    //アイテムをリストで受け取り、ミニマップ上に変換して描画
    void DrawItems(const std::vector<MapVector2>& items);

    // 円の範囲内に制限
    bool IsInsideCircle(int x, int y) const;

    // ミニマップ座標に変換
    MapVector2 WorldToMapPos(const MapVector2& worldPos) const;
};
