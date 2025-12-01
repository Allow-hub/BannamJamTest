# Stage関連のCPP分離が必要なファイル一覧

## 概要
Domain層以外のStage関連ファイルで、ヘッダーファイルに実装が含まれているため、cpp分離が必要なファイルの一覧。

## 分離対象ファイル

### 1. UseCase層

#### UseCase/StageService.h → StageService.cpp
- **場所**: `include/UseCase/StageService.h`
- **説明**: ステージサービスの中核クラス
- **実装内容**: 
  - `initialize()` メソッド - ステージ初期化処理
  - `update()` メソッド - 各ステージの更新処理
  - `syncPhysicsBodies()` メソッド - 物理ボディ同期処理
  - `getStages()`, `getPhysicsBodies()` - ゲッター
  - `clear()` メソッド - リソースクリア
  - `adjustPlayerOnGroundPlatform()` メソッド - プレイヤー位置調整
  - `drawPhysicsLayerDebug()` メソッド - デバッグ描画
- **注意事項**: ビルドエラーの原因となる可能性があるため、最後に実行することを推奨

### 2. Presentation層

#### Presentation/Stage/StageManager.h → StageManager.cpp
- **場所**: `include/Presentation/Stage/StageManager.h`
- **説明**: ステージ描画管理クラス
- **実装内容**:
  - `setService()` メソッド - サービス設定
  - `loadTextures()` メソッド - テクスチャ読み込み
  - `draw()` メソッド - 描画処理
  - `drawStage()` メソッド - 個別ステージ描画（private）

#### Presentation/Stage/BackgroundRenderer.h → BackgroundRenderer.cpp
- **場所**: `include/Presentation/Stage/BackgroundRenderer.h`
- **説明**: パララックス背景描画クラス
- **実装内容**:
  - `setBackgroundObjects()` メソッド - 背景オブジェクト設定
  - `drawLayer()` メソッド - レイヤー別描画
  - `isLoaded()`, `getObjectCount()` - 状態確認メソッド
  - `generateBackgroundInstances()` メソッド - 背景インスタンス生成（private）

### 3. Infrastructure層

#### Infrastructure/StageLoader.h → StageLoader.cpp
- **場所**: `include/Infrastructure/StageLoader.h`
- **説明**: ステージデータJSONローダー
- **実装内容**:
  - `loadStageFromFile()` メソッド（複数オーバーロード）- ステージファイル読み込み
  - `loadFromJson()` メソッド - JSON読み込み処理
  - `parseStageObject()` メソッド - ステージオブジェクト解析（private）
  - `parseRect()` メソッド - 矩形データ解析（private）

#### Infrastructure/Background/BackgroundLoader.h → BackgroundLoader.cpp
- **場所**: `include/Infrastructure/Background/BackgroundLoader.h`
- **説明**: 背景データJSONローダー
- **実装内容**:
  - `loadBackgroundFromFile()` メソッド（複数オーバーロード）- 背景ファイル読み込み
  - `parseBackgroundObject()` メソッド - 背景オブジェクト解析（private）

## 実装不要ファイル

### Infrastructure/StageFactory.h
- **理由**: 関数宣言のみで実装がヘッダーに含まれていない
- **状況**: 既にStageFactory.cppが存在している

## 作業順序の推奨

1. **Infrastructure層**: StageLoader, BackgroundLoader
2. **Presentation層**: StageManager, BackgroundRenderer  
3. **UseCase層**: StageService（最後に実行、ビルドエラーの可能性があるため）

## 注意事項

- 各cppファイル作成後は、Jam.vcxprojに`<ClCompile Include="..."/>`エントリを追加する必要がある
- StageService.cppの作成はPhysicsConverter.hの依存関係でビルドエラーが発生する可能性がある
- 実装内容は元のヘッダーファイルから正確にコピーし、ヘッダーから実装部分を削除する