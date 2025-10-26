# pragma once
# include <Siv3D.hpp>

// 飛翔物の状態
enum class ProjectileState
{
	Updating, //更新中
	Hit,      //何かにヒットした
	TimeOut,  //時間切れで消滅
};

class ProjectileBase
{
protected:

	//飛翔物の状態
	ProjectileState m_state = ProjectileState::Updating;

	//この飛翔物がまだ生存しているか
	bool m_isAlive = true;

public:

	ProjectileBase() = default;
	virtual ~ProjectileBase() = default;

	//毎フレームの更新処理
	//worl 物理ワールド
	virtual void update(P2World& world) = 0;

	//毎フレームの描画処理
	virtual void draw() const = 0;

	//この飛翔物が生存しているかを返す
	bool isAlive() const
	{
		return m_isAlive;
	}

	//現在の状態を返す
	//飛翔物の状態
	ProjectileState getState() const
	{
		return m_state;
	}

	// 将来的に「攻撃処理」側が参照するための情報を追加できます
	// (例: ヒットした位置、攻撃範囲、ダメージ量など)
};
