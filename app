#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"
EXE_PATH="$BUILD_DIR/UFO_Sheep_Abduction.exe"

if command -v g++ >/dev/null 2>&1; then
    CXX="g++"
elif command -v clang++ >/dev/null 2>&1; then
    CXX="clang++"
else
    echo "No C++ compiler found in PATH. Open MSYS2 UCRT64 and install mingw-w64-ucrt-x86_64-gcc or mingw-w64-ucrt-x86_64-clang." >&2
    exit 1
fi

if command -v pkg-config >/dev/null 2>&1; then
    GLEW_CFLAGS="$(pkg-config --cflags glew 2>/dev/null || true)"
    GLEW_LIBS="$(pkg-config --libs glew 2>/dev/null || echo -lglew32)"
    GLFW_LIBS="$(pkg-config --libs glfw3 2>/dev/null || echo -lglfw3)"
else
    GLEW_CFLAGS=""
    GLEW_LIBS="-lglew32"
    GLFW_LIBS="-lglfw3"
fi

mkdir -p "$BUILD_DIR"

"$CXX" -std=c++17 -O2 \
    -I"$ROOT_DIR" \
    -I"$ROOT_DIR/Model" \
    -I"$ROOT_DIR/Shader" \
    -I"$ROOT_DIR/glm" \
    $GLEW_CFLAGS \
    "$ROOT_DIR/main.cpp" \
    "$ROOT_DIR/Model/OBJFile.cpp" \
    "$ROOT_DIR/Model/ColoredMesh.cpp" \
    -o "$EXE_PATH" \
    $GLEW_LIBS $GLFW_LIBS -lopengl32 -lgdi32 -luser32 -lkernel32

cp -f "$ROOT_DIR/glew32.dll" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$ROOT_DIR/glfw3.dll" "$BUILD_DIR/" 2>/dev/null || true

# Ensure runtime assets are in the working directory used to launch the app.
cp -f "$ROOT_DIR/colorNormal.v" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$ROOT_DIR/colorNormal.f" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$ROOT_DIR/vshader.glsl" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$ROOT_DIR/cone.obj" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$ROOT_DIR/Ground/terrain.obj" "$BUILD_DIR/terrain.obj" 2>/dev/null || true
cp -f "$ROOT_DIR/Sheep/sheep.obj" "$BUILD_DIR/sheep.obj" 2>/dev/null || true
cp -f "$ROOT_DIR/UFO/ufo.obj" "$BUILD_DIR/ufo.obj" 2>/dev/null || true
cp -f "$ROOT_DIR/Scene/tree1.obj" "$BUILD_DIR/tree1.obj" 2>/dev/null || true

cd "$BUILD_DIR"
exec "$EXE_PATH"