#include "Domain/Stage/NormalStage.h"

namespace Jam::Domain::Stage {
    NormalStage::NormalStage(const StageObject& obj)
        : m_rect(obj.rect)
        , m_type(obj.type)
        , m_texturePath(obj.texturePath)
    {}
    
    // 静的なので何もしない
    void NormalStage::update(double deltaTime) {}
    
    RectF NormalStage::getRenderRect() const {
        return m_rect;
    }
    
    StageType NormalStage::getType() const {
        return m_type;
    }
    
    Vec2 NormalStage::getCurrentCenter() const {
        return m_rect.center();
    }
    
    // NormalStage固有のメソッド: テクスチャパスを取得
    String NormalStage::getTexturePath() const {
        return m_texturePath;
    }
}