# Enemyについて
Enemyは基本共通のAIを増やしていき、それをEnemyBaseを継承した具象クラスで組み立てます

## EnemyBase
Enemyに共通した機能を置きます（例、移動やAIの設定など）
衝突判定は一律下記の三つを継承しoverrideすれば中身を書けます
IphysicsBodyにはLayerが存在するのでそれで判定をしてください
	void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

## LittleDevil（Enemyの具象クラス）
Enemyの具象クラスはコンストラクタでAIを組み立ててください
ChaseAI,PatrolAIは使うからpushしようなど
それぞれのAIの特殊処理はベースでフックした処理を空実装しているのでそれをoverrideしてください
それぞれのAIは次のAIStateをしらないのでEventを起こすことができます
このEventを具象クラスで受け取り特定の処理をしてください

## IEnemyAI.h
すべてに共通するAIの基盤です（基本いじらないでください）

## ChaseAI.h
特定のものを追跡
PlayerはgetPlayerPos()でとれます

## PatrolAI.h
標的を探す