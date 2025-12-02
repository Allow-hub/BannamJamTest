# Domain - ゲームのコアビジネスロジックとステージシステム
Domain層はゲームの中心的なビジネスロジックを担当し、  
外部依存を持たない純粋なゲームルールとステージの概念を定義します。

## Domain層の共通データとインターフェース

### FlagmentMemory.h
- **役割**: 永続化されるべきフラグや状態を管理  
- **メリット**: セーブ/ロード処理を簡素化  
- **デメリット**: -  
- **制作者**: Allow-hub

### GoalArea.h
- **役割**: ゴールエリアのデータと処理  
- **メリット**: ゴール判定ロジックを集中管理  
- **デメリット**: -  
- **制作者**: Allow-hub

### IIndependentObject.h
- **役割**: 独立して動作するオブジェクトのインターフェース  
- **メリット**: 多態的な扱いを可能に  
- **デメリット**: -  
- **制作者**: Allow-hub

### IInputService.h
- **役割**: ユーザー入力の抽象化  
- **メリット**: 入力方法の変更に柔軟に対応  
- **デメリット**: -  
- **制作者**: Allow-hub

### ITakeDamageable.h
- **役割**: ダメージを受けるオブジェクトのインターフェース  
- **メリット**: ダメージ処理を統一的に扱える  
- **デメリット**: -  
- **制作者**: Allow-hub

### StoryData.h
- **役割**: ストーリー関連データ構造  
- **メリット**: 進行と表示を分離  
- **デメリット**: -  
- **制作者**: Allow-hub


## Domain層のEditor関連モデルとロジック

### EditorCameraController.h
- **役割**: エディタ内カメラ制御  
- **メリット**: ゲーム内カメラと独立  
- **デメリット**: -  
- **制作者**: Allow-hub

### EditorSettings.h
- **役割**: エディタ設定（グリッド、スナップなど）  
- **メリット**: 設定の一元管理  
- **デメリット**: -  
- **制作者**: Allow-hub

### EditorTypes.h
- **役割**: 共通型定義や列挙型  
- **メリット**: 型安全性と可読性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### EditorManagerBase.h
- **役割**: エディタ機能の基底クラス  
- **メリット**: 共通管理ロジックの再利用  
- **デメリット**: -  
- **制作者**: Allow-hub

### EnemyEditorManager.h
- **役割**: 敵エディタ管理  
- **メリット**: 編集ロジックを一元化  
- **デメリット**: -  
- **制作者**: Allow-hub

### EnemyEditorTypes.h
- **役割**: 敵エディタ型定義  
- **メリット**: 型安全性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### StageEditorManager.h
- **役割**: ステージエディタ管理  
- **メリット**: 編集ロジックを一元化  
- **デメリット**: -  
- **制作者**: Allow-hub

### StageEditorTypes.h
- **役割**: ステージエディタ型定義  
- **メリット**: 型安全性向上  
- **デメリット**: -  
- **制作者**: Allow-hub


## Domain層の敵キャラクターモデルとロジック  
Enemyは基本共通のAIを増やしていき、それをEnemyBaseを継承した具象クラスで組み立てます

### EnemyBase.h
- **役割**: 敵キャラクター共通基底クラス  
 - Enemyに共通した機能を置きます（例、移動やAIの設定など）  
 衝突判定は一律下記の三つを継承しoverrideすれば中身を書けます  
 IphysicsBodyにはLayerが存在するのでそれで判定をしてください  
	void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;  
	void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;  
	void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;  
- **メリット**: 共通ロジック再利用、多態的扱い  
- **デメリット**: クラス肥大化の可能性  
- **制作者**: Allow-hub

### EnemyType.h
- **役割**: 敵の種類識別  
- **メリット**: 型安全性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### Bomb.h, Clown.h, Eye.h, GothicLolitaDoll.h, LittleDevil.h, Ribbon.h, Spider.h
- **役割**: 各タイプの敵データと挙動  
- **メリット**: タイプ別ロジックのカプセル化  
- **デメリット**: クラス増加で管理煩雑化の可能性  
- **制作者**: Allow-hub

### EyeBeam.h, FireBall.h, Missile.h, ShockWave.h
- **役割**: 攻撃タイプのデータと挙動  
- **メリット**: 攻撃ロジックの再利用性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### Boss1_3.h
- **役割**: 特定ボスキャラの独自ロジック  
- **メリット**: 複雑ロジックを集中管理  
- **デメリット**: ボスごとにクラス増加  
- **制作者**: Allow-hub


## Domain層の敵AI

### IEnemyAI.h
- **役割**: AIインターフェース（基本いじらないでください）  
- **メリット**: 多様なAIロジックを統一的に扱える  
- **デメリット**: -  
- **制作者**: Allow-hub

### AISettings.h
- **役割**: AIパラメータ管理  
- **メリット**: 調整容易  
- **デメリット**: -  
- **制作者**: Allow-hub

### AttackAI.h, ChaseAI.h, PatrolAI.h, RunAwayAI.h
- **役割**: 各行動AI  
- **メリット**: 再利用性・交換可能性向上  
- **デメリット**: -  
- **制作者**: Allow-hub


## Domain層のゲームイベント

### GameEvents.h
- **役割**: ゲーム内イベント定義  
- **メリット**: オブジェクト間の疎結合  
- **デメリット**: イベント増加で追跡困難  
- **制作者**: Allow-hub


## Domain層の物理エンジン関連

### ICollisionListener.h
- **役割**: 
 - 当たり判定のイベントを受け取りたいクラスはこのインターフェースを継承します  
- **メリット**: 
 - Domain層のオブジェクトは「誰と当たったか」「当たったとき何をするか」だけに集中できる  
- **デメリット**: -  
- **制作者**: Allow-hub

### IPhysicsBody.h
- **役割**: 
 - 物理挙動に必要な関数（位置・速度の取得や力の適用など）を抽象化したインターフェース。 
- **メリット**:  
 - Domain層のオブジェクトは Siv3D などの具体的な物理APIを意識せずに操作できる  
- **デメリット**: -  
- **制作者**: Allow-hub

### PhysicsBodyID.h
- **役割**: 物理ボディ識別ID  
- **メリット**: 明確な参照  
- **デメリット**: -  
- **制作者**: Allow-hub

### PhysicsTypes.h
- **役割**: 物理オブジェクトの状態を表す構造体群  
- **メリット**: 型安全性向上  
- **デメリット**: -  
- **制作者**: Allow-hub


## Domain層のプレイヤーキャラクター

### Player.h
- **役割**: プレイヤーのコアデータと挙動  
- **メリット**: 属性と挙動を一元管理  
- **デメリット**: クラス肥大化の可能性  
- **制作者**: Allow-hub


## Domain層のチョーカー関連

### ChokerContext.h
- **役割**: チョーカー状態管理コンテキスト  
- **メリット**: 状態管理の一元化  
- **デメリット**: -  
- **制作者**: Allow-hub

### ChokerSkill.h
- **役割**: チョカースキルデータとロジック  
- **メリット**: カプセル化、再利用性・拡張性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### ChokerStates
- **ChokerFlyingState.h**: 空中状態  
- **ChokerIdleState.h**: 待機状態  
- **HookedEnemyState.h**: 敵フック状態  
- **HookedGroundState.h**: 地面フック状態  
- **IChokerState.h**: 状態インターフェース  
- **メリット**: 状態ごとの挙動をカプセル化、状態遷移明確化  
- **デメリット**: 状態増加で管理煩雑化  
- **制作者**: Allow-hub

### CustomDistanceJoint.h
- **役割**: チョーカー物理制約  
- **メリット**: 物理挙動カプセル化  
- **デメリット**: -  
- **制作者**: Allow-hub

## Domain層のステージ

### StageTypes.h, BackgroundTypes.h
- **役割**: 種類識別用列挙型  
- **メリット**: 型安全性向上  
- **デメリット**: -  
- **制作者**: Allow-hub

### IStage.h
- **役割**: ステージ要素インターフェース  
- **メリット**: 多態的扱い  
- **デメリット**: -  
- **制作者**: Allow-hub

### NormalStage.h, DamageStage.h, MovingPlatformStage.h, MovingDamagePlatformStage.h, OneWayPlatformStage.h
- **役割**: ステージ要素データとロジック  
- **メリット**: 複合挙動をカプセル化、再利用性向上  
- **デメリット**: -  
- **制作者**: Allow-hub
