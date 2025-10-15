# Stage System Enhancement - 動く床実装

##  概要
静的ステージから動的ステージへの拡張

##  実装タスク

### アーキテクチャ
- [ ] `IStage` インターフェース作成
- [ ] `MovingPlatformStage` 実装（`IStage` + `ICollisionListener`）
- [ ] コンストラクタ注入統一（`IPhysicsBody` 配列注入）
※Playerのコンストラクタを参考にするといい

### JSON拡張
- [ ] `movable` フラグ追加
- [ ] `movement` 設定追加（`speed`, `path`, `loop`）

### 統合
- [ ] `GameScene` で `IStage` 対応
- [ ] Player追従処理実装
- [ ] 動的ステージ更新ループ追加

## それができたら次やること
- マリオのようなすり抜ける床
・下からはいけるけど、乗ったら停止する
・乗っている状態で下にキーを押すとすり抜ける

- 壁を実際に壊す処理
・これも同様にIStageを継承して作成する