#pragma once
#include "../UseCase/EffectEvents.h"
#include "../UseCase/EffectImplementations.h"

using namespace Jam::UseCase;

namespace Jam::Presentation
{
	class EffectManager
	{
	private:
		Effect m_effect;
		EffectEventQueue& m_eventQueue;

	public:
		EffectManager(EffectEventQueue& eventQueue)
			: m_eventQueue(eventQueue)
		{
		}

		// イベントキューを処理してエフェクトを追加
		void processEvents()
		{
			while (!m_eventQueue.empty())
			{
				auto event = m_eventQueue.pop();

				std::visit([this](auto&& e) {
					using T = std::decay_t<decltype(e)>;

					if constexpr (std::is_same_v<T, StarEffectEvent>)
					{
						m_effect.add<StarEffect>(e);
					}
					else if constexpr (std::is_same_v<T, ExplosionEffectEvent>)
					{
						m_effect.add<ExplosionEffect>(e);
					}
					else if constexpr (std::is_same_v<T, ParticleEffectEvent>)
					{
						m_effect.add<ParticleEffect>(e);
					}
					else if constexpr (std::is_same_v<T, HitEffectEvent>)
					{
						m_effect.add<HitEffect>(e);
					}
					else if constexpr (std::is_same_v<T, TextEffectEvent>)
					{
						m_effect.add<TextEffect>(e);
					}
					else if constexpr (std::is_same_v<T, RingEffectEvent>)
					{
						m_effect.add<RingEffect>(e);
					}
				}, event);
			}
		}

		// エフェクトを更新・描画
		void update()
		{
			processEvents();
		}

		void draw()
		{
			m_effect.update();
		}

		// アクティブなエフェクトの数を取得
		size_t num_effects() const
		{
			return m_effect.num_effects();
		}

		// すべてのエフェクトをクリア
		void clear()
		{
			m_effect.clear();
		}
	};
}
