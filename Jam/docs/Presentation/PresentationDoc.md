# Presentation層の設計
Presentation層は 画面への描画やアニメーションの制御を担当します。  
Domain層やUseCase層の処理結果を元に、実際にプレイヤーや敵、背景などを画面に表示します。

### ✔ メリット
- Domain層のロジックと描画を分離できる
- 描画やアニメーションの変更が他層に影響しない


# Presentation層の共通管理クラス

## ⬛ モジュール一覧

### Animator.h
- 役割: アニメーションの再生と管理
- メリット: アニメーションロジックを一元管理できる
- デメリット: （記述なし）
- 制作者: Allow-hub

### AnimatorLoader.h
- 役割: アニメーションデータのロードとAnimatorへの供給
- メリット: ロード処理をカプセル化
- デメリット: （記述なし）
- 制作者: Allow-hub

### AudioService.h
- 役割: サウンドエフェクトやBGMの再生管理
- メリット: オーディオ処理の統一化
- デメリット: （記述なし）
- 制作者: Allow-hub

### CameraManager.h
- 役割: カメラの追従・視点変更・演出の制御
- メリット: カメラロジックの一元管理
- デメリット: （記述なし）
- 制作者: Allow-hub

### EffectManager.h
- 役割: パーティクルなど視覚エフェクトの生成・更新・描画
- メリット: エフェクトの管理効率化・パフォーマンス最適化
- デメリット: （記述なし）
- 制作者: Allow-hub

### EnemyManager.h
- 役割: 敵の描画とプレゼンテーション層での状態更新
- メリット: 敵描画の一元管理
- デメリット: （記述なし）
- 制作者: Allow-hub

### FadeManager.h
- 役割: フェードイン・フェードアウトの管理
- メリット: シーン遷移のビジュアル改善
- デメリット: （記述なし）
- 制作者: Allow-hub

### IndependentObjectManager.h
- 役割: 飛び道具など独立オブジェクトの描画と状態更新
- メリット: 管理の一元化
- デメリット: （記述なし）
- 制作者: Allow-hub

### InGameUIManager.h
- 役割: HPバーなどインゲームUIの描画  
- メリット: 情報表示とプレイ処理の分離  
- デメリット: （記述なし）  
- 制作者: Allow-hub,inuzami  

### PlayerManager.h
- 役割: プレイヤー描画とプレゼンテーション層での状態更新  
- メリット: プレイヤー描画処理の一元管理  
- デメリット: （記述なし）  
- 制作者: Allow-hub  

### ResourceManager.h
- 役割: テクスチャなどのロードと管理、キャッシュ
- メリット: メモリ管理の最適化
- デメリット: （記述なし）
- 制作者: Allow-hub

### SettingManager.h
- 役割: ゲーム設定（音量・など）の管理とUI制御
- メリット: ユーザー設定の統一的取り扱い
- デメリット: （記述なし）
- 制作者: Allow-hub

### StoryManager.h
- 役割: ストーリー演出とテキスト表示の管理
- メリット: ストーリーUIと制御の一元化
- デメリット: （記述なし）
- 制作者: Allow-hub

---

# Presentation層のエディタ関連描画

## Editorフォルダ

### EditorRendererBase.h
- 役割: エディタ描画の共通基底クラス
- メリット: 再利用性と拡張性の向上
- デメリット: （記述なし）
- 制作者: Wine-5

### EnemyEditorフォルダ / EnemyEditorRenderer.h
- 役割: エディタ上での敵の描画・編集UI
- メリット: 敵編集のUIとロジックの分離
- デメリット: （記述なし）
- 制作者: Wine-5

### StageEditorフォルダ / StageEditorRenderer.h
- 役割: ステージ配置・編集の視覚化
- メリット: ステージ編集処理のカプセル化
- デメリット: （記述なし）
- 制作者: Wine-5

### Utilitiesフォルダ
- EditorGridUtil.h  
  - 役割: グリッド描画ユーティリティ  
  - メリット: グリッド処理の共通化  
- EditorTextInputUtil.h  
  - 役割: テキスト入力UIユーティリティ  
  - メリット: 入力処理の共通化  
- 制作者: Wine-5

---

# Presentation層のポストエフェクト

### BloomManager.h
- 役割: ブルーム効果の適用
- メリット: 高度なグラフィック演出
- デメリット: 処理負荷が高くなる可能性、白飛びしやすい
- 制作者: Allow-hub


---

# Presentation層のシーン管理

### GameScene.h
- 役割: インゲームシーンの表示・更新
- メリット: シーン単位で管理しやすい
- デメリット: 複雑化,肥大化しやすい
- 制作者: Allow-hub

### ResultScene.h
- 役割: リザルト画面の描画・UI操作
- メリット: 結果と次の選択を提示
- 制作者: kinako

### SceneName.h
- 役割: シーン名定義（enum/定数）
- メリット: タイプミス防止・可読性向上

### SelectScene.h
- 役割: キャラ／ステージ選択画面の管理
- 制作者: kinako

### StageEditorScene.h
- 役割: ステージエディタのメインシーン管理
- 制作者: Wine-5

### StoryScene.h
- 役割: ストーリーイベント演出
- 制作者: Allow-hub

### TitleScene.h
- 役割: タイトル画面管理
- 制作者: Allow-hub

---

## Transitionフォルダ

- IrisTransition.h — 円形トランジション演出
- RectSlide.h — 矩形スライド演出
- ITransitionable.h — 遷移対応インターフェース
- TransitionManager.h — シーン遷移ロジック一元管理
- 制作者: kinako


---

# Presentation層のステージ描画

### BackgroundRenderer.h
- 役割: 背景描画・視差効果制御
- 制作者: Wine-5

### StageManager.h
- 役割: 背景／プラットフォーム等ステージ全体の描画管理
- 制作者: Wine-5
