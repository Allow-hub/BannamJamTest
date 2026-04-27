#include "Domain/Block/NormalBlock.h"

namespace Jam::Domain::Block {
    NormalBlock::NormalBlock(const BlockObject& obj)
        : m_rect(obj.rect)
        , m_type(obj.type)
        , m_texturePath(obj.texturePath)
    {}

    // 静的なので何もしない
    void NormalBlock::update(double deltaTime) {}

    RectF NormalBlock::getRenderRect() const {
        return m_rect;
    }

    BlockType NormalBlock::getType() const {
        return m_type;
    }

    Vec2 NormalBlock::getCurrentCenter() const {
        return m_rect.center();
    }

    // NormalBlock固有のメソッド: テクスチャパスを取得
    String NormalBlock::getTexturePath() const {
        return m_texturePath;
    }
}