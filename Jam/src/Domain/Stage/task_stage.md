# Stage System Enhancement - 動く床実装

##  概要
静的ステージから動的ステージへの拡張

##  実装タスク

### アーキテクチャ
- [ ] `IStage` インターフェース作成
- [ ] `MovingPlatformStage` 実装（`IStage` + `ICollisionListener`）
- [ ] コンストラクタ注入統一（`IPhysicsBody` 配列注入）

### JSON拡張
- [ ] `movable` フラグ追加
- [ ] `movement` 設定追加（`speed`, `path`, `loop`）

### 統合
- [ ] `GameScene` で `IStage` 対応
- [ ] Player追従処理実装
- [ ] 動的ステージ更新ループ追加