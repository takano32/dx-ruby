# dx-ruby

Ruby + DirectX 9 ゲームライブラリ。2009 年頃に作成された Windows 専用の独立プロジェクト。

## 背景

このプロジェクトは 2009 年頃、Visual Studio 6.0 / Ruby 1.8 / DirectX 9 の環境向けに作成された Windows 専用ゲームライブラリです。

| 項目 | 内容 |
|------|------|
| 当時の環境 | Windows XP / Ruby 1.8 / DirectX 9 SDK / Visual Studio 6.0 |
| 推奨動作環境 | Windows x86 / x64 + Ruby 3.3 for Windows (i386) |
| 代替動作環境 | Linux (aarch64) + Wine 10.x + Ruby 3.3 for Windows (i386) ※制限あり |

## モジュール構成

| モジュール | 機能 |
|-----------|------|
| `Window`   | Win32 ウィンドウ管理・メッセージループ |
| `Graphics` | DirectX 9 (D3D9/D3DX9) による 2D スプライト描画 |
| `Sound`    | DirectSound + ACM による BGM/SE 再生 (MP3対応) |
| `Input`    | DirectInput によるキーボード/ジョイスティック入力 |
| `Resource` | ファイル・ZIP アーカイブからのリソース読み込み |

## ビルド方法

### Windows (ネイティブ・推奨)

ネイティブ Windows (x86/x64) であればすべての機能が動作します。

**必要環境:**
- Ruby 3.3 for Windows i386 ([RubyInstaller](https://rubyinstaller.org/) + DevKit)
- DirectX 9 SDK (または Windows SDK)

```batch
rem 1. RubyInstaller + DevKit をインストール後
ridk install

rem 2. ビルド
make all

rem 3. インストール
make install

rem 4. デモ実行
make run
```

### Linux + Wine (aarch64)

ARM64 Linux + Wine でも動作しますが、Sound 再生に制限があります（後述）。

**必要環境:**
- Wine 10.x (Wayland 対応)
- Ruby 3.3 for Windows i386 — Wine prefix 内にインストール済み

**セットアップ:**

```bash
# 1. Wine prefix を作成し Ruby for Windows をインストール
WINEPREFIX=~/wine-dx-ruby wine rubyinstaller-devkit-3.3.7-1-x86.exe \
  /VERYSILENT /DIR="C:\\Ruby33"

# 2. ビルド
make all

# 3. インストール
make install

# 4. デモ実行 (eg/ をカレントにして Resource.new('data') を解決)
make run
```

> **ARM64 上のインストーラについて**
> Wine ARM64 WOW64 モードで 32-bit インストーラが起動に失敗する場合は、
> [rubyinstaller-3.3.7-1-x86.7z](https://github.com/oneclick/rubyinstaller2/releases)
> を 7z で展開して `C:\Ruby33` に手動配置し、
> MSYS2 の `mingw-w64-i686-gcc-libs` から `libstdc++-6.dll` / `libgcc_s_dw2-1.dll` を
> `C:\Ruby33\bin` に追加することで代替できます。

### 個別ビルド

```bash
make dx-ruby/window.so
make dx-ruby/graphics.so
make dx-ruby/sound.so
make dx-ruby/input.so
make dx-ruby/resource.so
```

ビルド成果物は `dx-ruby/` サブディレクトリに出力され、
`require 'dx-ruby/window'` のように名前空間付きでロードできます。

## Ruby API

```ruby
require 'dx-ruby/window'
require 'dx-ruby/graphics'
require 'dx-ruby/sound'
require 'dx-ruby/resource'
require 'dx-ruby/input'

# ウィンドウ作成
window = Window.new
window.create

# リソース読み込み (ディレクトリまたはZIPアーカイブ)
resource = Resource.new('data')

# DirectX 9 グラフィックス
graphics = Graphics.new(window)
img_data = resource.get('image.png')
graphics.regist_texture(1, img_data)
graphics.regist_sprite(1, 1, 0, 0, 256, 256)

# DirectSound
sound = Sound.new(window)
snd_data = resource.get('bgm.mp3')
sound.load(1, snd_data)
sound.play(1, true)  # ループ再生

# DirectInput
input = Input.new(window)

# ゲームループ
class MyAction
  def action
    input.update
    graphics.begin(0, 0, 0)
    graphics.draw_sprite(1, x, y, 0)
    graphics.end
  end
end

window.loop_action = MyAction.new
window.show
window.main
```

## コードの変更点 (Ruby 1.8 → Ruby 3.3)

歴史的なコードをほぼそのまま保ちつつ、以下の最小限の変更を行いました：

| 変更 | 内容 |
|------|------|
| `RSTRING(str)->ptr` | → `RSTRING_PTR(str)` |
| `RSTRING(str)->len` | → `RSTRING_LEN(str)` |
| `INT2FIX(hwnd)` | → `LONG2NUM((LONG)(ULONG_PTR)hwnd)` |
| `#pragma comment(lib, ...)` | 削除 (Makefileで明示指定) |
| `iostream.h` / `fstream.h` | → `iostream` / `fstream` (ISO C++) |
| `Window.h: Init_sound()` | → `Init_window()` (バグ修正) |
| `InputClass::Update()` | 戻り値型 `BOOL` を追加 (バグ修正) |
| `File.h`, `Log.h`, `Thread.h` | プロキシヘッダを追加 (パス解決) |
| `ResourceSystem::_isArchive` | 未初期化バグ修正 → コンストラクタで `false` に初期化 |
| `SoundBuffer::_pBuffer` | 未初期化バグ修正 → コンストラクタで `NULL` に初期化、各メソッドにNULLガード追加 |
| `require 'd4r/...'` | → `require 'dx-ruby/...'` に統一 |

## 既知の問題

### Linux + Wine (aarch64) 固有

- **Sound 再生クラッシュ**: `Sound#play` / `Sound#play_with_fade` を呼ぶと DirectSound の
  バックグラウンドオーディオスレッドが `kernel32.dll` 内でクラッシュする
  (`STATUS_ACCESS_VIOLATION` at `0x7b56f991`)。
  Wine 10.16 の ARM64 WOW64 モードで 32-bit DirectSound スレッドが正常起動しない
  **Wine 側の制限**。ネイティブ Windows では発生しない。
  `Sound#load` まで・Window / Graphics / Input は問題なく動作する。
- **D3DX ミップマップフィルタ**: Wine の D3DX9 実装が一部フィルタ未対応
  (`d3dx_load_pixels_from_pixels Unhandled filter`)。
  テクスチャのミップマップ品質が低下するが動作には支障なし。
  ネイティブ Windows では発生しない。

### 全環境共通

- **Sound モジュール終了時クラッシュ**: 元のコードの既知の問題
  (`TODO: delete時に落ちることがある`)。BGM フェードスレッドのクリーンアップが未実装。
- **UNZIP32.DLL**: ZIP アーカイブ機能には日本語 ZIP DLL (UNZIP32.DLL) が必要。
  ない場合はディレクトリからの直接読み込みにフォールバックする。

## ライセンス

歴史的なコード。元のライセンス情報なし。
