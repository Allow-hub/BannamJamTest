#pragma once
#include "Domain/Stage/IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 動く床
     * 横移動、縦移動、円運動の3種類の動作パターンを持つ
     */
    class MovingPlatformStage : public IStage {
    private:
        RectF m_baseRect;              // 基準矩形
        Vec2 m_baseCenter;             // 基準中心位置
        Vec2 m_currentOffset;          // 現在のオフセット
        MovementType m_movementType;   // 移動タイプ
        double m_movementSpeed;        // 移動速度(ピクセル/秒)
        double m_movementDistance;     // 移動距離または半径
        bool m_loopMovement;           // ループするか
        double m_elapsedTime;          // 経過時間
        
    public:
        MovingPlatformStage(const StageObject& obj);
        
        void update(double deltaTime) override;
        
        RectF getRenderRect() const override;
        
        StageType getType() const override;
        
        Vec2 getCurrentCenter() const override;
        
    private:
        // 横移動の更新
        void updateHorizontal(double deltaTime);
        
        // 縦移動の更新
        void updateVertical(double deltaTime);
        
        // 円運動の更新
        void updateCircular(double deltaTime);
    };
}