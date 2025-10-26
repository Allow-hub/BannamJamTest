# 今多 悠人のやることリスト

---

## 敵関連

### Bossの作成

#### 仕様概要
ボスの挙動として、以下のような構成を予定

1. **追尾ミサイルを発射**  
   - プレイヤーを追尾するミサイルを放つ。

2. **ミサイル反射ギミック**  
   - プレイヤーがミサイルを跳ね返すと、ボスに命中。  
   - 命中後、弱点が出現。

3. **チョーカー突撃フェーズ**  
   - 弱点出現中に「チョーカー突撃」で攻撃。  
   - この流れを3回繰り返すとクリア。

#### 攻撃パターン追加（予定）
- 上記以外に1〜2種類の攻撃パターンを追加予定。  
- 攻撃のバリエーションを増やし、戦闘に緊張感を持たせる。

---

## ステージ関連

### 動く床のバリエーションを増やす
1. 縦に動く床
2. 円を描くような床

→ jsonファイルで管理できるようにする

---

## Stageのリファクタリング

### 方針
- ❌ `StageRenderer`を削除 → `StageManager`に統合
- ✅ `IStage`の`update()`, `draw()`を各層で呼ぶだけにする
- ✅ `StageService`は`update()`のみの処理を書く
- ✅ `StageManager`は`draw()`のみ担当
- ✅ それぞれのStageクラス（`NormalStage`など）は個別の処理のみを持つ

### 必要なクラス一覧

#### Domain層
- `IStage` （インターフェース）
- `NormalStage` （具象クラス）
- `MovingPlatformStage` （具象クラス）
- `StageTypes` （型定義）

#### UseCase層
- `StageService`

#### Infrastructure層
- `StageLoader`
- `StageFactory`

#### Presentation層
- `StageManager`

---

## クラス設計詳細

### Domain層

```
Jam/src/Domain/Stage/
├── IStage.h/cpp
│   └── 役割: ステージの抽象インターフェース
│       - virtual void update(double deltaTime) = 0
│       - virtual RectF getRenderRect() const = 0
│       - virtual StageType getType() const = 0
│       - virtual std::shared_ptr<Physics::IPhysicsBody> getPhysicsBody() const = 0
│
├── NormalStage.h/cpp
│   └── 役割: 通常の静的ステージ
│       - 物理ボディを保持
│       - update()は何もしない（静的なため）
│       - 描画情報を提供
│
├── MovingPlatformStage.h/cpp
│   └── 役割: 動く床
│       - 直線移動、円運動などの移動ロジック
│       - update()で位置を更新
│       - 物理ボディの位置を同期
│
└── StageTypes.h
    └── 役割: ステージ関連の型定義
        - enum class StageType (Normal, MovingPlatform, etc.)
        - struct StageObject (rect, type, movementData)
        - struct MovementPattern (type, speed, range, etc.)
```

### UseCase層

```
Jam/src/UseCase/
└── StageService.h/cpp
    └── 役割: ステージのビジネスロジック統括
        - Array<unique_ptr<IStage>> m_stages を管理
        - void update(double deltaTime) ← 全ステージ更新
        - const Array<unique_ptr<IStage>>& getStages() const
        - bool initializeStage(filename, world)
          └─ m_stages = StageFactory::createStagesFromFile(filename, world);
```

### Infrastructure層

```
Jam/src/Infrastructure/
├── StageLoader.h/cpp
│   └── 役割: JSONファイルからステージデータ読み込み
│       - static bool loadStageFromFile(filename, out Array<StageObject>)
│       - JSONパース処理
│       - エラーハンドリング
│
└── StageFactory.h/cpp
    └── 役割: StageObjectから具象Stageインスタンス配列を生成
        - static Array<unique_ptr<IStage>> createStagesFromFile(filename, P2World&)
        - 内部でStageLoaderを呼び出してJSONを読み込む
        - StageObjectの配列からIStageの配列に変換
        - private static unique_ptr<IStage> createStage(StageObject, P2World&)
          ├─ StageTypeに応じた適切なクラスを生成
          └─ 物理ボディの生成処理
```

### Presentation層

```
Jam/src/Presentation/Stage/
└── StageManager.h/cpp
    └── 役割: ステージの描画管理
        - shared_ptr<StageService> m_stageService（参照のみ）
        - HashTable<StageType, Texture> m_stageTextures
        - void draw() const ← 描画のみ（updateなし）
        - void loadTextures() ← テクスチャ読み込み
```

---

## アーキテクチャ図

```
┌─────────────────────────────────────────────────────┐
│               Presentation層                         │
│  ┌───────────────────┐                              │
│  │  StageManager     │ ← draw()のみ                │
│  └─────────┬─────────┘                              │
└────────────┼──────────────────────────────────────────┘
             │ 参照（読み取り専用）
┌────────────▼──────────────────────────────────────────┐
│               UseCase層                               │
│  ┌───────────────────┐                              │
│  │  StageService     │ ← update()実行               │
│  │                   │   　　　　　　　　　          │
│  └─────────┬─────────┘                                │
└────────────┼──────────────────────────────────────────┘
             │ 使用
┌────────────┼──────────────────────────────────────────┐
│            │         Infrastructure層                 │
│  ┌─────────▼─────┐     ┌──────────────┐            │
│  │ StageLoader   │     │StageFactory  │            │
│  │ (static)      │     │ 　　　　     │  Array<IStage>管理          │
│  └───────────────┘     └──────┬───────┘            │
└─────────────────────────────┼──────────────────────┘
                              │ 生成
┌─────────────────────────────▼──────────────────────┐
│                    Domain層                         │
│  ┌──────────┐                                       │
│  │  IStage  │ ← インターフェース                    │
│  └────┬─────┘                                       │
│       │ implements                                   │
│  ┌────┴─────────────────────┐                      │
│  │                           │                      │
│  ▼                           ▼                      │
│ ┌──────────────┐  ┌─────────────────────┐         │
│ │ NormalStage  │  │MovingPlatformStage │         │
│ └──────────────┘  └─────────────────────┘         │
│                                                      │
│  ┌──────────────┐                                   │
│  │ StageTypes   │ ← 型定義                         │
│  └──────────────┘                                   │
└─────────────────────────────────────────────────────┘
```

---

## 各層の責務まとめ

| 層 | クラス | 主な責務 | メソッド |
|---|---|---|---|
| **Domain** | `IStage` | ステージの振る舞い定義 | `update()`, `getRenderRect()` |
| **Domain** | `NormalStage` | 静的ステージ実装 | 物理ボディ保持 |
| **Domain** | `MovingPlatformStage` | 動く床実装 | 移動ロジック |
| **Domain** | `StageTypes` | 型定義 | 列挙型、構造体 |
| **UseCase** | `StageService` | ビジネスロジック<br>配列の管理と更新 | `update()`, `initializeStage()` |
| **Infrastructure** | `StageLoader` | JSON読み込み | `loadStageFromFile()` |
| **Infrastructure** | `StageFactory` | 配列の生成 | `createStagesFromFile()` |
| **Presentation** | `StageManager` | 描画管理 | `draw()` ← **updateなし** |

---

## 実装の流れ

1. ✅ `StageTypes.h`を定義
2. ✅ `IStage`インターフェースを作成
3. ✅ `NormalStage`, `MovingPlatformStage`を実装
4. ✅ `StageLoader`でJSON読み込み機能を実装
5. ✅ `StageFactory`で配列生成機能を実装
   - `createStagesFromFile()`でLoaderを呼び出し
   - StageObjectからIStageへ変換
6. ✅ `StageService`で配列管理と`update()`実行
   - Factoryから配列を受け取る
7. ✅ `StageManager`で`draw()`のみ実装

---

## 設計のポイント

### Factoryがstaticである理由
- ✅ **状態を持たない**: 純粋な生成ロジックのみを担当
- ✅ **インスタンス不要**: わざわざ`StageFactory factory;`を作る必要がない
- ✅ **シンプルな呼び出し**: `StageFactory::createStagesFromFile(...)`で完結
- ✅ **一貫性**: `StageLoader`もstaticなので統一

### Array<IStage>の管理について
- **生成**: `StageFactory`がまとめて配列を生成
- **管理**: `StageService`がライフサイクルと更新を管理
- **参照**: `StageManager`が描画用に読み取り専用で参照

```cpp
// 責務の分担
StageFactory::createStagesFromFile()  // ← 配列を生成して返す
    ↓
StageService::m_stages = ...         // ← 配列を管理・更新
    ↓
StageManager::draw()                 // ← 配列を参照して描画
```