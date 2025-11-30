# ドキュメントをコマンドラインで確認するツール
##  概要
ドキュメントをわざわざmdファイルを探して開くのが面倒だと感じたので作りました  
ターミナルで確認したいクラスをコマンドで入力すると出力できます  
.mdファイルは現在のディレクトリの直下の中のdocsフォルダにある必要があります  
また出力したいクラス名は必ず「### クラス名」である必要があります

##　使い方
 dotnet doc 確認したいクラス名.h  
 をターミナルで打つと以下のように出力されます
 

## 更新方法
ツール自体の更新方法  
dotnet pack tools/DocCli -o nupkg  
dotnet tool update --add-source ./nupkg doccli --no-cache  

プロジェクトメンバー側の更新  
### 1. パッケージをビルド
dotnet pack tools/DocCli -o nupkg

### 2. ツールをインストール
dotnet tool restore

### 3.出力
dotnet doc 確認したいクラス名.h  