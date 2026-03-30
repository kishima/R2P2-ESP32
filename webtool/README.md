# R2P2-ESP32 Web Flasher

ブラウザから直接ESP32にファームウェアをフラッシュできるWebツールです。

## 特徴

- ブラウザから直接ESP32へファームウェアをフラッシュ
- シリアルコンソール機能
- ケーブル接続だけで使用可能（ドライバ不要）
- クロスプラットフォーム対応

## 必要な環境

### ブラウザ

Web Serial APIをサポートするブラウザが必要です：
- Google Chrome 89+
- Microsoft Edge 89+
- Opera 75+

**注意**: Firefox と Safari は現在 Web Serial API に対応していません。

### ハードウェア

- ESP32開発ボード
- USBケーブル

## 使い方

### 1. ファームウェアのビルド

まず、ファームウェアをビルドします：

```bash
# プロジェクトルートで実行
rake build
```

これにより、以下のファイルが生成されます：
- `build/bootloader/bootloader.bin`
- `build/partition_table/partition-table.bin`
- `build/R2P2-ESP32.bin`
- `build/storage.bin`

### 2. ローカルサーバーの起動

Web Flasherツールを起動します：

```bash
# プロジェクトルートまたはwebtoolディレクトリで実行
./webtool/server.sh
```

このスクリプトは自動的に：
1. `build/` ディレクトリから必要なファイルをコピー
2. `webtool/firmware/` ディレクトリに配置
3. Webサーバーを起動

デフォルトでは、ポート8000でサーバーが起動します。別のポートを使用する場合：

```bash
./webtool/server.sh 8080
```

### 3. ブラウザでアクセス

ブラウザで以下のURLを開きます：

```
http://localhost:8000/
```

### 4. ESP32の接続とフラッシュ

1. ESP32をUSBケーブルでPCに接続
2. ブラウザで「Connect and Flash」ボタンをクリック
3. シリアルポートを選択するダイアログが表示されるので、ESP32のポートを選択
4. 「Install R2P2-ESP32」をクリック
5. フラッシュが開始されます（通常1-2分程度）
6. 完了したら「Close」をクリック

### 5. シリアルコンソールの使用

フラッシュ完了後、シリアルコンソールを使用できます：

1. 「Connect to Serial」ボタンをクリック
2. ESP32のシリアルポートを選択
3. コンソールにログが表示されます
4. 下部の入力欄からコマンドを送信できます

## トラブルシューティング

### ポートが表示されない

- ESP32が正しく接続されているか確認
- USBケーブルがデータ転送に対応しているか確認
- デバイスマネージャーでポートが認識されているか確認

### フラッシュに失敗する

- ESP32のBOOTボタンを押しながら「Connect and Flash」を実行
- 別のUSBポートを試す
- USBケーブルを交換する
- ボーレートを下げる（manifest.jsonで設定可能）

### ブラウザで開けない

- 対応ブラウザ（Chrome/Edge/Opera）を使用しているか確認
- HTTPSではなくHTTPでアクセスしているか確認（localhost除く）

## ファイル構成

```
webtool/
├── index.html        # メインのWebインターフェース
├── manifest.json     # フラッシュするファイルの定義
├── server.sh         # ローカルサーバー起動スクリプト
├── firmware/         # ビルドされたファームウェアファイル (自動生成)
│   ├── bootloader.bin
│   ├── partition-table.bin
│   ├── R2P2-ESP32.bin
│   └── storage.bin
└── README.md         # このファイル
```

**注意**: `firmware/` ディレクトリは `server.sh` 実行時に自動的に作成され、ビルドファイルがコピーされます。

## manifest.jsonのカスタマイズ

フラッシュするファイルやオフセットを変更する場合、`manifest.json`を編集します：

```json
{
  "name": "R2P2-ESP32",
  "builds": [
    {
      "chipFamily": "ESP32",
      "parts": [
        {
          "path": "firmware/bootloader.bin",
          "offset": 4096
        },
        {
          "path": "firmware/R2P2-ESP32.bin",
          "offset": 65536
        }
      ]
    }
  ]
}
```

**注意**: パスは `webtool/` ディレクトリからの相対パスです（サーバーがwebtoolで起動するため）。

## 技術情報

### ESP Web Tools

このツールは [ESP Web Tools](https://esphome.github.io/esp-web-tools/) を使用しています。

### Web Serial API

Web Serial APIについての詳細：
- [MDN Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
- [Chrome Platform Status](https://chromestatus.com/feature/6577673212002304)

### フラッシュアドレス

デフォルトのパーティションレイアウト：

| ファイル | オフセット (hex) | オフセット (dec) | サイズ |
|---------|-----------------|-----------------|--------|
| bootloader | 0x1000 | 4096 | ~26KB |
| partition table | 0x8000 | 32768 | 3KB |
| app | 0x10000 | 65536 | ~2MB |
| storage | 0x210000 | 2162688 | 1MB |

## ライセンス

このプロジェクトのライセンスに従います。

## 関連リンク

- [R2P2-ESP32 プロジェクト](../)
- [PicoRuby](https://github.com/picoruby/picoruby)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
