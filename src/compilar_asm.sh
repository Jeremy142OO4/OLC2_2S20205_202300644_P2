#!/usr/bin/env bash
set -euo pipefail

# ========= Configuración =========
AS_BIN=${AS_BIN:-aarch64-linux-gnu-as}
LD_BIN=${LD_BIN:-aarch64-linux-gnu-ld}
QEMU_BIN=${QEMU_BIN:-qemu-aarch64}

SRC="../salida.s"   # ← un nivel arriba
OBJ="../salida.o"
BIN="../salida"

# ========= Validaciones =========
if [[ ! -f "$SRC" ]]; then
  echo "❌ No se encontró el archivo $SRC"
  exit 1
fi

if ! command -v "$AS_BIN" &>/dev/null; then
  echo "❌ Falta ${AS_BIN}. Instálalo con: sudo apt install gcc-aarch64-linux-gnu"
  exit 1
fi

if ! command -v "$LD_BIN" &>/dev/null; then
  echo "❌ Falta ${LD_BIN}. Instálalo con: sudo apt install binutils-aarch64-linux-gnu"
  exit 1
fi

# ========= Ensamblar =========
echo "🧩 Ensamblando $SRC -> $OBJ"
"$AS_BIN" -o "$OBJ" "$SRC"

# ========= Enlazar =========
echo "🔗 Enlazando -> $BIN"
"$LD_BIN" -o "$BIN" "$OBJ" -e _start --build-id=none

# ========= Éxito =========
echo "✅ Ensamblado y enlazado correctamente."
echo "Binario generado: $BIN"

# ========= Ejecutar opcional =========
if command -v "$QEMU_BIN" &>/dev/null; then
  echo "▶ Ejecutando con $QEMU_BIN ..."
  "$QEMU_BIN" "$BIN"
else
  echo "⚠ No se encontró qemu-aarch64. Para probar el binario, instálalo con:"
  echo "   sudo apt install qemu-user"
fi