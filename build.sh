#!/bin/bash
# dx-ruby build script
# Uses Wine to run the MinGW GCC from RubyInstaller DevKit
# Produces Windows i386 DLLs (.so) for Ruby 3.3

set -e

WINEPREFIX=/home/takano32/wine-dx-ruby
export WINEPREFIX

RUBY_WIN="C:\\Ruby33"
DEVKIT="C:\\Ruby33\\msys32\\mingw32"
GCC="${DEVKIT}\\bin\\i686-w64-mingw32-g++.exe"

# Unix paths for include/lib (used in wine paths)
RUBY_INCLUDE="${WINEPREFIX}/drive_c/Ruby33/include/ruby-3.3.0"
DEVKIT_INC="${WINEPREFIX}/drive_c/Ruby33/msys32/mingw32/include"
DEVKIT_LIB="${WINEPREFIX}/drive_c/Ruby33/msys32/mingw32/lib"
RUBY_LIB="${WINEPREFIX}/drive_c/Ruby33/lib"

# Source root (converted to Windows path for Wine)
SRC_WIN="Z:${PWD}"

CXXFLAGS="-shared -O2 -std=c++14 \
  -I${SRC_WIN//\//\\\\}\\Util \
  -I${RUBY_INCLUDE//\//\\\\} \
  -I${RUBY_INCLUDE//\//\\\\}\\i386-mingw32 \
  -L${RUBY_LIB//\//\\\\} \
  -L${DEVKIT_LIB//\//\\\\}"

wine_build() {
  local module=$1
  local extra_inc=$2
  local sources=$3
  local libs=$4

  echo "=== Building ${module}.so ==="

  # Convert paths
  local win_inc="${SRC_WIN//\//\\\\}\\${extra_inc//\//\\\\}"
  local win_src=""
  for f in $sources; do
    win_src="${win_src} ${SRC_WIN//\//\\\\}\\${f//\//\\\\}"
  done

  wine "$GCC" \
    ${CXXFLAGS} \
    -I"${win_inc}" \
    ${win_src} \
    ${libs} \
    -lmsvcrt-ruby330.dll \
    -o "${SRC_WIN//\//\\\\}\\${module}.so" \
    2>&1 | grep -v "^00\|fixme\|err:wayland\|experimental" || true

  if [ -f "${module}.so" ]; then
    echo "  -> ${module}.so OK"
  else
    echo "  -> ${module}.so FAILED"
  fi
}

# --- Window ---
wine_build "window" "Window" \
  "Window/Window.cpp Window/WindowClass.cpp" \
  ""

# --- Graphics ---
wine_build "graphics" "Graphics" \
  "Graphics/Graphics.cpp Graphics/GraphicsClass.cpp Graphics/GraphicsFader.cpp Graphics/GraphicsFont.cpp Graphics/GraphicsSprite.cpp Graphics/GraphicsSystem.cpp Graphics/GraphicsTexture.cpp Util/Log.cpp" \
  "-ld3d9 -ld3dx9 -ldxguid"

# --- Sound ---
wine_build "sound" "Sound" \
  "Sound/Sound.cpp Sound/SoundClass.cpp Sound/SoundSystem.cpp Sound/SoundBuffer.cpp Sound/SoundACM.cpp Sound/SoundFadeRequest.cpp Sound/SoundFadeThread.cpp Util/Log.cpp" \
  "-ldsound -ldxguid"

# --- Input ---
wine_build "input" "Input" \
  "Input/Input.cpp Input/InputClass.cpp Input/InputSystem.cpp Input/InputDevice.cpp Input/KeyboardDevice.cpp Input/JoystickDevice.cpp Util/Log.cpp" \
  "-ldinput -ldxguid"

# --- Resource ---
wine_build "resource" "Resource" \
  "Resource/Resource.cpp Resource/ResourceClass.cpp Resource/ResourceSyetem.cpp" \
  ""

echo ""
echo "=== Installing .so files ==="
OUTDIR="${WINEPREFIX}/drive_c/Ruby33/lib/ruby/site_ruby/3.3.0"
for f in window.so graphics.so sound.so input.so resource.so; do
  if [ -f "$f" ]; then
    cp "$f" "${OUTDIR}/"
    echo "  Installed: ${OUTDIR}/${f}"
  fi
done

echo ""
echo "=== Done ==="
