# Infrastructure 層の設計

Infrastructure層は **外部APIと他層へのアダプター的役割** を持つ<br> 
Domain層やUseCase層が外部ライブラリ（Siv3Dや物理エンジン）に依存しないように橋渡しを行う

---

## PhysicsConverter.h

- **役割**：
  - Domain層の `PhysicsMaterial` を Siv3D の `P2Material` に変換
  - Player のステータスを JSON からロードする関数も提供
- **メリット**：
  - Domain層は物理APIの型を知らずに済む
  - JSONロードなどの外部依存処理をまとめて管理できる

---

## Siv3DInputManager.h

- **役割**：
  - Siv3D の入力 (`Input` クラス) を Domain層の `IInputService` に適合させるアダプター
- **メリット**：
  - Domain層や UseCase層は Siv3D の API を直接呼ばない
  - 入力ライブラリを変更しても Domain層に影響を与えない

---

## Siv3DPhysicsBody.h

- **役割**：
  - Domain層の `IPhysicsBody` と Siv3D の物理エンジンを適合させる
  - 当たり判定のイベント通知も担当（`ICollisionListener` に通知）
- **メリット**：
  - Domain層のオブジェクトはどの物理エンジンを使っているか知らない
  - 物理の処理や衝突判定を Infrastructure層に委譲できる

---
