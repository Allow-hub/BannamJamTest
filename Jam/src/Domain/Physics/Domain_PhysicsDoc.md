# Domain::Physics 層の設計

Domain層のオブジェクトが **物理APIに直接依存しないようにするための抽象クラス群** 
この設計により、後で物理エンジンを差し替えたとしても Domain層のコードには影響がない

---

## ICollisionListener

- **役割**：
  当たり判定のイベントを受け取りたいクラスはこのインターフェースを継承します。
- **仕組み**：
  実際の衝突検出やイベント発火は Infrastructure 層（今回なら `Siv3DPhysicsBody`）が担当し、
  Domain層のオブジェクトに通知します。
- **メリット**：
  Domain層のオブジェクトは「誰と当たったか」「当たったとき何をするか」だけに集中できる。

---

## IPhysicsBody

- **役割**：
  物理挙動に必要な関数（位置・速度の取得や力の適用など）を抽象化したインターフェース。
- **メリット**：
  Domain層のオブジェクトは Siv3D などの具体的な物理APIを意識せずに操作できる。
- **例**：
  - `applyForce`：力を加える
  - `applyImpulse`：衝撃を加える
  - `getPosition` / `setTransform`：位置・回転を取得・設定

---

## PhysicsTypes

- **役割**：
  物理オブジェクトの状態を表す構造体群。
- **内容**：
  - `PhysicsTransform`：位置や回転を保持
  - `PhysicsMaterial` ：摩擦や反発係数などの物理特性
  - `PhysicsLayer`    ：物理の判定用レイヤー
- **メリット**：
  Domain層のロジックは物理的なデータ構造に直接依存せず、抽象化された型を使える。
