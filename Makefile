# dx-ruby Makefile
# ビルドターゲット: Ruby 3.3 for Windows (i386-mingw32) under Wine
# 使用方法:
#   make all     - 全モジュールをビルド
#   make install - Ruby site_rubyにインストール
#   make run     - デモを実行
#   make clean   - ビルド成果物を削除

WINEPREFIX    = /home/takano32/wine-dx-ruby
WINEPATH      = C:\\Ruby33\\msys32\\mingw32\\bin;C:\\Ruby33\\bin
RUBY_WIN      = $(WINEPREFIX)/drive_c/Ruby33
DEVKIT        = $(RUBY_WIN)/msys32/mingw32
GCC           = C:\\Ruby33\\msys32\\mingw32\\bin\\i686-w64-mingw32-g++.exe
RUBY_EXE      = C:\\Ruby33\\bin\\ruby.exe

# プロジェクトルートのZ:ドライブパス
SRC_Z         = Z:/home/takano32/GitHub/dx-ruby
SRC_W         = Z:\\home\\takano32\\GitHub\\dx-ruby

# インクルードパス (Windows パス形式)
INCLUDES      = \
  -I"$(SRC_W)\\Util" \
  -I"$(SRC_W)" \
  -I"C:\\Ruby33\\include\\ruby-3.3.0" \
  -I"C:\\Ruby33\\include\\ruby-3.3.0\\i386-mingw32" \
  -I"C:\\Ruby33\\msys32\\mingw32\\include"

# ライブラリパス
LIBDIRS       = \
  -L"C:\\Ruby33\\lib" \
  -L"C:\\Ruby33\\msys32\\mingw32\\lib"

# 共通フラグ
CXXFLAGS      = -shared -O2 -std=c++14
WIN32_LIBS    = -lmsvcrt-ruby330.dll -lgdi32 -luser32 -lkernel32

# Wine 実行コマンド
WINE_CMD      = WINEPREFIX=$(WINEPREFIX) WINEPATH="$(WINEPATH)" wine $(GCC) \
                $(CXXFLAGS) $(INCLUDES) $(LIBDIRS)

# インストール先
OUTDIR        = $(RUBY_WIN)/lib/ruby/site_ruby/3.3.0

.PHONY: all install run clean

all: window.so graphics.so sound.so input.so resource.so

# --- Window ---
window.so:
	$(WINE_CMD) \
	  -I"$(SRC_W)\\Window" \
	  "$(SRC_W)\\Window\\Window.cpp" \
	  "$(SRC_W)\\Window\\WindowClass.cpp" \
	  $(WIN32_LIBS) \
	  -o "$(SRC_W)\\window.so"

# --- Graphics ---
graphics.so:
	$(WINE_CMD) \
	  -I"$(SRC_W)\\Graphics" \
	  "$(SRC_W)\\Graphics\\Graphics.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsClass.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsFader.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsFont.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsSprite.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsSystem.cpp" \
	  "$(SRC_W)\\Graphics\\GraphicsTexture.cpp" \
	  "$(SRC_W)\\Util\\Log.cpp" \
	  $(WIN32_LIBS) -ld3d9 -ld3dx9 -ldxguid \
	  -o "$(SRC_W)\\graphics.so"

# --- Sound ---
sound.so:
	$(WINE_CMD) \
	  -I"$(SRC_W)\\Sound" \
	  "$(SRC_W)\\Sound\\Sound.cpp" \
	  "$(SRC_W)\\Sound\\SoundClass.cpp" \
	  "$(SRC_W)\\Sound\\SoundSystem.cpp" \
	  "$(SRC_W)\\Sound\\SoundBuffer.cpp" \
	  "$(SRC_W)\\Sound\\SoundACM.cpp" \
	  "$(SRC_W)\\Sound\\SoundFadeRequest.cpp" \
	  "$(SRC_W)\\Sound\\SoundFadeThread.cpp" \
	  "$(SRC_W)\\Util\\Log.cpp" \
	  "$(SRC_W)\\Util\\Thread.cpp" \
	  "$(SRC_W)\\Util\\File.cpp" \
	  $(WIN32_LIBS) -ldsound -ldxguid -lwinmm -lmsacm32 \
	  -o "$(SRC_W)\\sound.so"

# --- Input ---
input.so:
	$(WINE_CMD) \
	  -I"$(SRC_W)\\Input" \
	  "$(SRC_W)\\Input\\Input.cpp" \
	  "$(SRC_W)\\Input\\InputClass.cpp" \
	  "$(SRC_W)\\Input\\InputSystem.cpp" \
	  "$(SRC_W)\\Input\\InputDevice.cpp" \
	  "$(SRC_W)\\Input\\KeyboardDevice.cpp" \
	  "$(SRC_W)\\Input\\JoystickDevice.cpp" \
	  "$(SRC_W)\\Util\\Log.cpp" \
	  $(WIN32_LIBS) -ldinput -ldxguid \
	  -o "$(SRC_W)\\input.so"

# --- Resource ---
resource.so:
	$(WINE_CMD) \
	  -I"$(SRC_W)\\Resource" \
	  "$(SRC_W)\\Resource\\Resource.cpp" \
	  "$(SRC_W)\\Resource\\ResourceClass.cpp" \
	  "$(SRC_W)\\Resource\\ResourceSyetem.cpp" \
	  $(WIN32_LIBS) \
	  -o "$(SRC_W)\\resource.so"

# インストール
install: all
	install -d $(OUTDIR)
	install -m 644 window.so graphics.so sound.so input.so resource.so $(OUTDIR)/
	@echo "Installed to $(OUTDIR)/"

# デモ実行
run: install
	WINEPREFIX=$(WINEPREFIX) WINEPATH="$(WINEPATH)" \
	wine $(RUBY_EXE) "$(SRC_W)\\eg\\graphics_demo.rbw"

# クリーン
clean:
	rm -f window.so graphics.so sound.so input.so resource.so
