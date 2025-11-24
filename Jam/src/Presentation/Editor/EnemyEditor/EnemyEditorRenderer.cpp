#include "EnemyEditorRenderer.h"

namespace Jam::Presentation::Editor
{
    int EnemyEditorRenderer::drawCurrentMode(int y) const
    {
        y = this->drawSectionHeader(U"現在のモード", y);
        
        String modeName;
        ColorF modeColor;
        switch (this->m_service->getMode())
        {
        case UseCase::Editor::EditorMode::Select:
            modeName = U"選択モード";
            modeColor = Palette::Cyan;
            break;
        case UseCase::Editor::EditorMode::Place:
            modeName = U"配置モード";
            modeColor = Palette::Lime;
            break;
        case UseCase::Editor::EditorMode::Delete:
            modeName = U"削除モード";
            modeColor = Palette::Red;
            break;
        }
        
        this->m_font(modeName).draw(this->getPanelX() + 15, y, modeColor);
        y += 45;
        
        y += this->getItemSpacing();
        return y;
    }
    
    int EnemyEditorRenderer::drawEnemyTypeSelector(int y) const
    {
        y = this->drawSectionHeader(U"敵タイプ", y);
        
        String currentTypeName = getEnemyTypeName(this->m_service->getEnemyType());
        if (SimpleGUI::Button(currentTypeName, Vec2{this->getPanelX() + 10, y}, 280))
        {
            m_isEnemyTypeDropdownOpen = !m_isEnemyTypeDropdownOpen;
        }
        y += 40;
        
        if (m_isEnemyTypeDropdownOpen)
        {
            Array<Domain::Editor::EnemyType> types = {
                Domain::Editor::EnemyType::LittleDevil,
                Domain::Editor::EnemyType::Spider,
                Domain::Editor::EnemyType::Ribbon,
                Domain::Editor::EnemyType::Eye,
                Domain::Editor::EnemyType::Clown,
                Domain::Editor::EnemyType::Boss1_3
            };
            
            for (const auto& type : types)
            {
                if (SimpleGUI::Button(getEnemyTypeName(type), Vec2{this->getPanelX() + 15, y}, 265))
                {
                    this->m_service->setEnemyType(type);
                    m_isEnemyTypeDropdownOpen = false;
                }
                y += 35;
            }
            y += 10;
        }
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int EnemyEditorRenderer::drawPatrolSettings(int y) const
    {
        if (this->m_service->getEnemyType() == Domain::Editor::EnemyType::Boss1_3)
            return y;
            
        y = this->drawSectionHeader(U"巡回設定", y);
        
        // 巡回距離
        double patrolDistance = this->m_service->getPatrolDistance();
        if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(patrolDistance), patrolDistance, 50.0, 500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setPatrolDistance(patrolDistance);
            m_patrolDistanceTextEdit.text = Format(static_cast<int>(patrolDistance));
        }
        y += 35;
        
        if (m_patrolDistanceTextEdit.text.isEmpty() || !m_patrolDistanceTextEdit.active)
        {
            m_patrolDistanceTextEdit.text = Format(static_cast<int>(this->m_service->getPatrolDistance()));
        }
        
        const int patrolDistanceTextY = y;
        if (SimpleGUI::TextBox(m_patrolDistanceTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_patrolDistanceTextEdit.text))
            {
                this->m_service->setPatrolDistance(Clamp(*value, 50.0, 500.0));
            }
        }
        
        if (m_patrolDistanceTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_patrolDistanceTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, patrolDistanceTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_patrolDistanceTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        // 待機時間
        double waitTime = this->m_service->getPatrolWaitTime();
        if (SimpleGUI::Slider(U"待機: {:.1f}s"_fmt(waitTime), waitTime, 0.5, 5.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setPatrolWaitTime(waitTime);
            m_waitTimeTextEdit.text = Format(waitTime, 1);
        }
        y += 35;
        
        if (m_waitTimeTextEdit.text.isEmpty() || !m_waitTimeTextEdit.active)
        {
            m_waitTimeTextEdit.text = Format(this->m_service->getPatrolWaitTime(), 1);
        }
        
        const int waitTimeTextY = y;
        if (SimpleGUI::TextBox(m_waitTimeTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_waitTimeTextEdit.text))
            {
                this->m_service->setPatrolWaitTime(Clamp(*value, 0.5, 5.0));
            }
        }
        
        if (m_waitTimeTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_waitTimeTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, waitTimeTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_waitTimeTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        // 発見距離
        double foundDistance = this->m_service->getFoundDistance();
        if (SimpleGUI::Slider(U"発見: {:.0f}"_fmt(foundDistance), foundDistance, 300.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setFoundDistance(foundDistance);
            m_foundDistanceTextEdit.text = Format(static_cast<int>(foundDistance));
        }
        y += 35;
        
        if (m_foundDistanceTextEdit.text.isEmpty() || !m_foundDistanceTextEdit.active)
        {
            m_foundDistanceTextEdit.text = Format(static_cast<int>(this->m_service->getFoundDistance()));
        }
        
        const int foundDistanceTextY = y;
        if (SimpleGUI::TextBox(m_foundDistanceTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_foundDistanceTextEdit.text))
            {
                this->m_service->setFoundDistance(Clamp(*value, 300.0, 1000.0));
            }
        }
        
        if (m_foundDistanceTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_foundDistanceTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, foundDistanceTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_foundDistanceTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int EnemyEditorRenderer::drawChaseSettings(int y) const
    {
        if (this->m_service->getEnemyType() == Domain::Editor::EnemyType::Boss1_3 ||
            this->m_service->getEnemyType() == Domain::Editor::EnemyType::LittleDevil)
            return y;
            
        y = this->drawSectionHeader(U"追跡設定", y);
        
        // 攻撃範囲
        double attackRange = this->m_service->getAttackRange();
        if (SimpleGUI::Slider(U"攻撃範囲: {:.0f}"_fmt(attackRange), attackRange, 300.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setAttackRange(attackRange);
            m_attackRangeTextEdit.text = Format(static_cast<int>(attackRange));
        }
        y += 35;
        
        if (m_attackRangeTextEdit.text.isEmpty() || !m_attackRangeTextEdit.active)
        {
            m_attackRangeTextEdit.text = Format(static_cast<int>(this->m_service->getAttackRange()));
        }
        
        const int attackRangeTextY = y;
        if (SimpleGUI::TextBox(m_attackRangeTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_attackRangeTextEdit.text))
            {
                this->m_service->setAttackRange(Clamp(*value, 300.0, 1000.0));
            }
        }
        
        if (m_attackRangeTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_attackRangeTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, attackRangeTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_attackRangeTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        // 見失い範囲
        double loseRange = this->m_service->getLoseRange();
        if (SimpleGUI::Slider(U"見失い範囲: {:.0f}"_fmt(loseRange), loseRange, 500.0, 1500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setLoseRange(loseRange);
            m_loseRangeTextEdit.text = Format(static_cast<int>(loseRange));
        }
        y += 35;
        
        if (m_loseRangeTextEdit.text.isEmpty() || !m_loseRangeTextEdit.active)
        {
            m_loseRangeTextEdit.text = Format(static_cast<int>(this->m_service->getLoseRange()));
        }
        
        const int loseRangeTextY = y;
        if (SimpleGUI::TextBox(m_loseRangeTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_loseRangeTextEdit.text))
            {
                this->m_service->setLoseRange(Clamp(*value, 500.0, 1500.0));
            }
        }
        
        if (m_loseRangeTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_loseRangeTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, loseRangeTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_loseRangeTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        // 速度倍率
        double speedFactor = this->m_service->getMoveSpeedFactor();
        if (SimpleGUI::Slider(U"速度倍率: {:.1f}"_fmt(speedFactor), speedFactor, 1.0, 2.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setMoveSpeedFactor(speedFactor);
            m_speedFactorTextEdit.text = Format(speedFactor, 1);
        }
        y += 35;
        
        if (m_speedFactorTextEdit.text.isEmpty() || !m_speedFactorTextEdit.active)
        {
            m_speedFactorTextEdit.text = Format(this->m_service->getMoveSpeedFactor(), 1);
        }
        
        const int speedFactorTextY = y;
        if (SimpleGUI::TextBox(m_speedFactorTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_speedFactorTextEdit.text))
            {
                this->m_service->setMoveSpeedFactor(Clamp(*value, 1.0, 2.0));
            }
        }
        
        if (m_speedFactorTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_speedFactorTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, speedFactorTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_speedFactorTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        y += this->getSmallSpacing();
        return y;
    }
    
    String EnemyEditorRenderer::getEnemyTypeName(Domain::Editor::EnemyType type) const
    {
        switch (type)
        {
        case Domain::Editor::EnemyType::LittleDevil: return U"リトルデビル";
        case Domain::Editor::EnemyType::Spider: return U"スパイダー";
        case Domain::Editor::EnemyType::Ribbon: return U"リボン";
        case Domain::Editor::EnemyType::Eye: return U"アイ";
        case Domain::Editor::EnemyType::Clown: return U"クラウン";
        case Domain::Editor::EnemyType::Boss1_3: return U"ボス1-3";
        default: return U"不明";
        }
    }
    
    ColorF EnemyEditorRenderer::getEnemyColor(Domain::Editor::EnemyType type) const
    {
        switch (type)
        {
        case Domain::Editor::EnemyType::LittleDevil: return ColorF{1.0, 0.5, 0.5};
        case Domain::Editor::EnemyType::Spider: return ColorF{0.5, 0.3, 0.8};
        case Domain::Editor::EnemyType::Ribbon: return ColorF{1.0, 0.7, 0.8};
        case Domain::Editor::EnemyType::Eye: return ColorF{0.3, 0.8, 1.0};
        case Domain::Editor::EnemyType::Clown: return ColorF{0.9, 0.9, 0.3};
        case Domain::Editor::EnemyType::Boss1_3: return ColorF{1.0, 0.0, 0.0};
        default: return Palette::Gray;
        }
    }
    
    void EnemyEditorRenderer::drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const
    {
        const ColorF color = getEnemyColor(enemy.data.type);
        const double radius = enemy.data.isBoss() ? 50.0 : 30.0;
        
        Circle{enemy.data.position, radius}.draw(color.withAlpha(0.6));
        Circle{enemy.data.position, radius}.drawFrame(3.0, isSelected ? ColorF(Palette::Yellow) : color);
        
        this->m_smallFont(getEnemyTypeName(enemy.data.type))
            .drawAt(enemy.data.position.movedBy(0, radius + 15), Palette::White);
    }
    
    void EnemyEditorRenderer::drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const
    {
        const double patrolDistance = enemy.patrol.patrolPoints.size() >= 2 ? 
            enemy.patrol.patrolPoints[0].position.distanceFrom(enemy.patrol.patrolPoints[1].position) : 100.0;
        
        if (enemy.patrol.patrolPoints.size() >= 2)
        {
            for (size_t i = 0; i < enemy.patrol.patrolPoints.size(); ++i)
            {
                const auto& point = enemy.patrol.patrolPoints[i];
                Circle{point.position, 8.0}.draw(ColorF{0.0, 1.0, 0.0, 0.5});
                
                if (i + 1 < enemy.patrol.patrolPoints.size())
                {
                    Line{point.position, enemy.patrol.patrolPoints[i + 1].position}
                        .draw(2.0, ColorF{0.0, 1.0, 0.0, 0.7});
                }
            }
            
            if (enemy.patrol.loop && enemy.patrol.patrolPoints.size() > 0)
            {
                Line{enemy.patrol.patrolPoints.back().position, enemy.patrol.patrolPoints.front().position}
                    .draw(2.0, ColorF{0.0, 1.0, 0.0, 0.7});
            }
        }
        
        if (enemy.chase)
        {
            Circle{enemy.position, enemy.chase->attackRange}.drawFrame(2.0, ColorF{1.0, 0.0, 0.0, 0.5});
            Circle{enemy.position, enemy.chase->loseRange}.drawFrame(2.0, ColorF{1.0, 1.0, 0.0, 0.3});
        }
    }
}