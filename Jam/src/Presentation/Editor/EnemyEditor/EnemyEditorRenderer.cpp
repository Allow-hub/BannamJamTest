#include <Siv3D.hpp>
#include "EnemyEditorRenderer.h"

namespace Jam::Presentation::Editor
{
    void EnemyEditorRenderer::init(UseCase::Editor::EnemyEditorService* service)
    {
        m_service = service;
    }

    void EnemyEditorRenderer::drawGUIPanel() const
    {
        const int panelX = Scene::Width() - 300;
        Rect{panelX, 0, 300, Scene::Height()}.draw(ColorF{0.1, 0.1, 0.1, 0.9});
        
        int y = 20;
        
        m_font(U"[敵エディタ]").draw(panelX + 10, y, Palette::White);
        y += 40;
        
        if (SimpleGUI::Button(U"ステージエディタに切り替え", Vec2{panelX + 10, y}, 280))
        {
            m_switchToStageEditor = true;
        }
        y += 45;
        
        SimpleGUI::Headline(U"敵タイプ", Vec2{panelX + 10, y});
        y += 40;
        
        String currentTypeName = getEnemyTypeName(m_service->getEnemyType());
        if (SimpleGUI::Button(currentTypeName, Vec2{panelX + 10, y}, 280))
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
                if (SimpleGUI::Button(getEnemyTypeName(type), Vec2{panelX + 10, y}, 280))
                {
                    m_service->setEnemyType(type);
                    m_isEnemyTypeDropdownOpen = false;
                }
                y += 40;
            }
        }
        
        bool isBoss = (m_service->getEnemyType() == Domain::Editor::EnemyType::Boss1_3);
        
        if (!isBoss)
        {
            SimpleGUI::Headline(U"巡回設定", Vec2{panelX + 10, y});
            y += 40;
            
            double patrolDistance = m_service->getPatrolDistance();
            if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(patrolDistance), patrolDistance, 50.0, 500.0, Vec2{panelX + 10, y}, 120, 120))
            {
                m_service->setPatrolDistance(patrolDistance);
            }
            y += 40;
            
            double waitTime = m_service->getPatrolWaitTime();
            if (SimpleGUI::Slider(U"待機: {:.1f}s"_fmt(waitTime), waitTime, 0.5, 5.0, Vec2{panelX + 10, y}, 120, 120))
            {
                m_service->setPatrolWaitTime(waitTime);
            }
            y += 40;
            
            double foundDistance = m_service->getFoundDistance();
            if (SimpleGUI::Slider(U"発見: {:.0f}"_fmt(foundDistance), foundDistance, 300.0, 1000.0, Vec2{panelX + 10, y}, 120, 120))
            {
                m_service->setFoundDistance(foundDistance);
            }
            y += 40;
            
            bool hasChaseAI = (m_service->getEnemyType() != Domain::Editor::EnemyType::LittleDevil);
            
            if (hasChaseAI)
            {
                SimpleGUI::Headline(U"追跡設定", Vec2{panelX + 10, y});
                y += 40;
                
                double attackRange = m_service->getAttackRange();
                if (SimpleGUI::Slider(U"攻撃範囲: {:.0f}"_fmt(attackRange), attackRange, 300.0, 1000.0, Vec2{panelX + 10, y}, 120, 120))
                {
                    m_service->setAttackRange(attackRange);
                }
                y += 40;
                
                double loseRange = m_service->getLoseRange();
                if (SimpleGUI::Slider(U"見失い範囲: {:.0f}"_fmt(loseRange), loseRange, 500.0, 1500.0, Vec2{panelX + 10, y}, 120, 120))
                {
                    m_service->setLoseRange(loseRange);
                }
                y += 40;
                
                double speedFactor = m_service->getMoveSpeedFactor();
                if (SimpleGUI::Slider(U"速度倍率: {:.1f}"_fmt(speedFactor), speedFactor, 1.0, 2.0, Vec2{panelX + 10, y}, 120, 120))
                {
                    m_service->setMoveSpeedFactor(speedFactor);
                }
                y += 40;
            }
        }
        
        SimpleGUI::Headline(U"操作", Vec2{panelX + 10, y});
        y += 40;
        m_smallFont(U"クリック: 敵を配置").draw(panelX + 10, y, Palette::White);
        y += 25;
    }

    void EnemyEditorRenderer::drawEnemies() const
    {
        auto transformer = m_service->createCameraTransformer();
        
        const auto& enemies = m_service->getEnemyManager().getAllEnemies();
        
        for (const auto& enemy : enemies)
        {
            drawEnemy(enemy, enemy.isSelected);
            
            if (!enemy.enemyObject.isBoss())
            {
                drawPatrolRange(enemy.enemyObject);
            }
        }
    }

    void EnemyEditorRenderer::drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const
    {
        const Vec2& pos = enemy.enemyObject.position;
        const ColorF color = getEnemyColor(enemy.enemyObject.type);
        
        Circle{pos, 20}.draw(color);
        Circle{pos, 20}.drawFrame(2.0, isSelected ? Palette::Yellow : Palette::Black);
        
        m_smallFont(getEnemyTypeName(enemy.enemyObject.type)).drawAt(pos.x, pos.y - 30, Palette::White);
    }

    void EnemyEditorRenderer::drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const
    {
        if (enemy.patrol.patrolPoints.size() < 2) return;
        
        const Vec2& basePos = enemy.position;
        const ColorF lineColor{1.0, 0.5, 0.0, 0.8};
        
        Vec2 point1 = basePos + enemy.patrol.patrolPoints[0].position;
        Vec2 point2 = basePos + enemy.patrol.patrolPoints[1].position;
        
        Line{point1, point2}.draw(3.0, lineColor);
        Circle{point1, 5}.draw(lineColor);
        Circle{point2, 5}.draw(lineColor);
    }

    String EnemyEditorRenderer::getEnemyTypeName(Domain::Editor::EnemyType type) const
    {
        switch (type)
        {
        case Domain::Editor::EnemyType::LittleDevil: return U"小悪魔";
        case Domain::Editor::EnemyType::Spider: return U"蜘蛛";
        case Domain::Editor::EnemyType::Ribbon: return U"リボン";
        case Domain::Editor::EnemyType::Eye: return U"目玉";
        case Domain::Editor::EnemyType::Clown: return U"道化師";
        case Domain::Editor::EnemyType::Boss1_3: return U"ボス1-3";
        default: return U"不明";
        }
    }

    ColorF EnemyEditorRenderer::getEnemyColor(Domain::Editor::EnemyType type) const
    {
        switch (type)
        {
        case Domain::Editor::EnemyType::LittleDevil: return ColorF{1.0, 0.5, 0.5};
        case Domain::Editor::EnemyType::Spider: return ColorF{0.5, 0.3, 0.1};
        case Domain::Editor::EnemyType::Ribbon: return ColorF{1.0, 0.7, 0.8};
        case Domain::Editor::EnemyType::Eye: return ColorF{0.7, 0.7, 1.0};
        case Domain::Editor::EnemyType::Clown: return ColorF{1.0, 1.0, 0.5};
        case Domain::Editor::EnemyType::Boss1_3: return ColorF{0.8, 0.2, 0.2};
        default: return Palette::Gray;
        }
    }
}
