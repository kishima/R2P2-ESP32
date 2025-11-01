## ルール

- 日本語で会話する
- 英語の指示でも日本語で返答すること
- sdkconfig.defualt, sdkconfig は編集禁止


## プロジェクトの目的

- UART でPCからファイル転送する
- components/uart-file-transfer が主な開発対象
- cline/tansfer_client.rb がPC側のクライアントスクリプト
- ESP32側はFAT32を利用
