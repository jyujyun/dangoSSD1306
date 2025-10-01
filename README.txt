**dangoSSD1306**
2025/10/01 3shokudango

このライブラリはRP2040でSSD1306を制御するために作られました。
RP2350でもできるかもね。

導入手順
このライブラリはPicoSDK上で使うことを前提としています
VisualStudioでRaspberryPiPicoの拡張機能をインストールするのが一番簡単です。
二つのファイルをプロジェクトのフォルダーに入れてください。
Makefileにこのライブラリのファイルとi2cのライブラリを追加するのを忘れないでください。

ヘッダファイルで制御するピンやSSD1306のアドレスを変更できます。

関数一覧
oledInit(void)
SSD1306をリセットします。

oledClear(data)
画面を一色で埋めます。
画面を消す場合は0x00、埋める場合は0xFFを指定してください。

oledSend(void)
RP2040内部のバッファーからSSD1306に転送します。
この関数を実行しない限り画面は更新されません。

oledDraw8(x,y,bmp[])
画面のx,yに8x8のビットマップ画像を表示します。

oledPixel(x,y,light)
画面のx,yに1ドット打ち込みます。
lightが1で点灯、0で消灯です。

oledLine(x1,y1,x2,y2)
画面のx1,y1からx2,y2に向かって直線を引きます。

oledFullString(x,y,string)
画面のx,yに文字列を描きます。
一応ひらがなに対応しています。カタカナは無理です。

oledNumber(x,y,number)
画面のx,yに数字を描きます。

oledDrawFill(bmp[])
画面いっぱいのビットマップ画像(128x64)を描きます。

最後に
SSD1306などのOLED向けのビットマップ画像を簡単に作れるアプリを作りました。
ST7735などのカラーLCDにも対応しています。ぜひダウンロードしてね。
https://3shokudango.web.fc2.com/hexbin.html
