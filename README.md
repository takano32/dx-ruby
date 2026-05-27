# dx-ruby

Ruby + DirectX 9 ゲームライブラリ。2009 年頃に作成された Windows 専用の独立プロジェクト。

## 背景

このプロジェクトは 2009 年頃、Visual Studio 6.0 / Ruby 1.8 / DirectX 9 の環境向けに作成された Windows 専用ゲームライブラリです。

| 項目 | 内容 |
|------|------|
| 当時の環境 | Windows XP / Ruby 1.8 / DirectX 9 SDK / Visual Studio 6.0 |
| 現在の動作環境 | Linux (aarch64) + Wine 10.x + Ruby 3.3 for Windows (i386) |

## モジュール構成

| モジュール | 機能 |
|-----------|------|
| `Window`   | Win32 ウィンドウ管理・メッセージループ |
| `Graphics` | DirectX 9 (D3D9/D3DX9) による 2D スプライト描画 |
| `Sound`    | DirectSound + ACM による BGM/SE 再生 (MP3対応) |
| `Input`    | DirectInput によるキーボード/ジョイスティック入力 |
| `Resource` | ファイル・ZIP アーカイブからのリソース読み込み |

## ビルド方法 (Linux + Wine)

### 必要環境

- Wine 10.x (Wayland 対応)
- Ruby 3.3 for Windows i386 (RubyInstaller + DevKit) — Wine prefix 内にインストール済み

### セットアップ

```bash
# 1. Wine prefix に Ruby for Windows をインストール
WINEPREFIX=~/wine-dx-ruby wine rubyinstaller-devkit-3.3.7-1-x86.exe \
  /VERYSILENT /DIR="C:\\Ruby33"

# 2. ビルド
make all

# 3. インストール
make install

# 4. デモ実行
make run
```

### 個別ビルド

```bash
make window.so
make graphics.so
make sound.so
make input.so
make resource.so
```

## Ruby API

```ruby
require 'window'
require 'graphics'
require 'sound'
require 'resource'
require 'input'

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

## 既知の問題

- **Sound 再生クラッシュ (ARM64 + Wine WOW64)**: `Sound#play` / `Sound#play_with_fade` を呼ぶと
  DirectSound のバックグラウンドオーディオスレッドが `kernel32.dll` 内
  (`0x7b56f991` / `STATUS_ACCESS_VIOLATION`) でクラッシュする。
  Wine 10.16 の ARM64 WOW64 モードで 32-bit DirectSound スレッドが正常起動しない Wine 側の制限。
  `Sound#load` までは動作する。グラフィックスとインプットは影響なし。
- **Sound モジュール終了時クラッシュ**: 元のコードの既知の問題 (`TODO: delete時に落ちることがある`)。BGMスレッドのクリーンアップが未実装。
- **UNZIP32.DLL**: ZIP アーカイブ機能には日本語 ZIP DLL (UNZIP32.DLL) が必要。ない場合はディレクトリからの直接読み込みにフォールバック。
- **表示**: Wayland 環境では Wine の Waylanddrv を使用。X11 環境では通常の DirectX 表示が利用可能。

## ライセンス

歴史的なコード。元のライセンス情報なし。
