# hw
A simple program that read a QR code jpg file and decode the URL inside it.

# Development

參考 quirc 的 tests/qrtest.c 做修改，設計一個簡單的 decoder 程式。每次呼叫時，透過第一個參數指定要解碼的 QR code jpg 檔案。

.c 文件位於 src/MainEntry.c

# How to build

## Build

程式是在 openSUSE Tumbleweed x64 上使用 gcc 編譯，所依賴的第三方 libjpeg、quirc 已上傳到 3rd 目錄下。

```
$ git clone https://github.com/wcchou/hw.git
$ cd hw/src
$ make
```

若是在非 openSUSE Tumbleweed x64 可能會遭遇錯誤。請參考以下編譯方式：

### libjpeg

```
$ cd hw/3rd
$ git clone https://github.com/LuaDist/libjpeg.git
$ cd libjpeg
$ ./configuration --PREFIX=local-path-to-hw/3rd/libjpeg
$ make
$ make install
```

### quirc

quirc 依賴於 libjpeg ，需先確保 libjpeg 已編譯完成。另外 quirc 依賴 libpng、libSDL、libSDL_gfx ，建議安裝該些套件，或是手動修改 Makefile 亦可（稍微麻煩點）

```
$ cd hw/3rd
$ git clone https://github.com/dlbeer/quirc.git
$ cd quirc
$ make LDFLAGS=-Llocal-path-to-hw/3rd/libjpeg/lib64
$ make PREFIX=local-path-to-hw/3rd/quirc install
```

## Clean
make clean

# Test

測試資料存放在 test\ 目錄下，測試資料是以 https://lubn.com/、http://www.google.com 為測試資料去生成各自的 QR code 後以 jpeg 存放，並加以變形。

1. google.jpg: 將 http://www.google.com 編碼
2. lubn.jpeg：將 https://lubn.com/ 編碼
3. google_90.jpg：將 http://www.google.com 編碼後，向右旋轉 90 度
4. google_180.jpg：將 http://www.google.com 編碼後，向右旋轉 180 度
5. google_270.jpg：將 http://www.google.com 編碼後，向右旋轉 270 度
6. merge_h.jpg：將 http://www.google.com 、https://lubn.com/ 各自編碼生成的圖片做水平合併
7. merge_v.jpg：將 http://www.google.com 、https://lubn.com/ 各自編碼生成的圖片做重值合併

```
$ decoder jpg-file-to-be-decoded
```
