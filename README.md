# hw
A simple program that read a QR code jpg file and decode the URL inside it.

# Design
基本上是參考 quirc 的 tests/qrtest.c 做修改，設計一個簡單的 decoder 程式。每次呼叫時，透過第一個參數指定要解碼的 QR code jpg 檔案。.c 文件位於 src/MainEntry.c

## decode_file()
在進行 QR code 解碼時，需向 quirc 申請產生一個 quirc ojbect，並且需要有對應的 quirc_new()、quirc_destroy() ，因此 quirc object 的管理集中在 decode_file() 中，並將讀取 jpeg 檔案、解碼的動作抽離到 decode_via_obj() ，透過減量 quirc_new()、quirc_destroy() 間的程式碼數目，避免 resource leakage 。

```
    struct quirc* pDecoderObj = quirc_new();
    int ret_code = decode_via_obj( pDecoderObj, pFilePath );
    quirc_destroy( pDecoderObj );
```

## decode_via_obj()

quirc 解碼方式，是將 jpeg 文件內容放入 quirc object 內後才進行解碼。因此先透過 libjpeg 取得指定的圖片大小（read_jpeg_file()），並在 quirc object 內透過 quirc_resize() 準備好對應的記憶體。之後可透過 quirc_begin() 取得配置好的記憶體位置開頭。並透過 libjpeg 把圖片資料一行一行放入 quirc object 內。

圖片內容都填入 quirc object 後，呼叫 quirc_end() 便可進行解碼。

解碼結果可能包含多個 QR codes 。因此可以透過 quirc_count() + loop 的方式去迭代。每次迭代中，透過 quirc_extract() 取得單筆 QR code 結果。

quirc object 是個可重複利用的 object ，但因為本程式一次只處理一張 jpeg 圖片，因此

## Usage

程式名為 decoder ，執行時須指定一個參數作為 jpg 文件名，參數不足或解碼失敗會有錯誤訊息輸出。

```
$ decoder jpg-file-to-be-decoded
```

# How to build

## Build

程式是在 openSUSE Tumbleweed x64 上使用 gcc 編譯，所依賴的第三方 libjpeg、quirc 已上傳到 3rd 目錄下。

```
$ git clone https://github.com/wcchou/hw.git
$ cd hw/src
$ make
```

編譯完成後，可在 hw/src 下生成 decoder 。

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

測試資料存放在 test\ 目錄下，測試資料是以 https://lubn.com/、http://www.google.com 為測試資料去生成各自的 QR code 後以 jpeg 存放，並加以變形，目前使用以下資料測試：

1. google.jpg: 將 http://www.google.com 編碼
2. lubn.jpeg：將 https://lubn.com/ 編碼
3. google_90.jpg：將 http://www.google.com 編碼後，向右旋轉 90 度
4. google_180.jpg：將 http://www.google.com 編碼後，向右旋轉 180 度
5. google_270.jpg：將 http://www.google.com 編碼後，向右旋轉 270 度
6. merge_h.jpg：將 http://www.google.com 、https://lubn.com/ 各自編碼生成的圖片做水平合併
7. merge_v.jpg：將 http://www.google.com 、https://lubn.com/ 各自編碼生成的圖片做重值合併

測試方法就是檢查是否能輸出對應的 URL 即可，如：

```
$ decoder ../test/google_90.jpg
$ [decode] data: http://www.google.com 
```
