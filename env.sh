#!/usr/bin/env bash

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  echo "You need to source this file"
  echo "  source env_osdev.sh"
  exit 1
fi

export OSDEV_ROOT="$(pwd)"
export TARGET=i386-elf
export PREFIX="$OSDEV_ROOT/toolchain"

export TARGET_CC="$TARGET-gcc"
export TARGET_LD="$TARGET-ld"
export TARGET_AS="nasm"
export TARGET_QEMU="qemu-system-i386"

_OLD_PATH="$PATH"
_OLD_PS1="$PS1"
export PATH="$PREFIX/bin:$PATH"
export PS1="(osdev) $PS1"

deactivate() {
  if ! [[ -z $_OLD_PS1 ]]; then
    export PS1="$_OLD_PS1"
  fi
  if ! [[ -z $_OLD_PATH ]]; then
    export PATH="$_OLD_PATH"
  fi

}

NASM_VERSION=2.16.03
BINUTILS_VERSION=2.43
GCC_VERSION=14.2.0
QEMU_VERSION=10.0.2

function install_assembler() {
  if [[ -f "$PREFIX/bin/$TARGET_AS" ]]; then
    return
  fi

  mkdir -p "$PREFIX/src"
  cd "$PREFIX/src"

  wget https://www.nasm.us/pub/nasm/releasebuilds/$NASM_VERSION/nasm-$NASM_VERSION.tar.gz

  tar xzvf nasm-$NASM_VERSION.tar.gz

  cd "nasm-$NASM_VERSION"
  ./configure --prefix="$PREFIX"
  make -j$(nproc)
  make install
  cd ..
  rm -fr "nasm-$NASM_VERSION" "nasm-$NASM_VERSION.tar.gz"
  cd $OSDEV_ROOT
}

function install_compiler() {
  if [[ -f "$PREFIX/bin/$TARGET_CC" ]]; then
    return
  fi

  mkdir -p "$PREFIX/src"
  cd "$PREFIX/src"

  wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz

  tar xzvf "gcc-$GCC_VERSION.tar.gz"
  
  cd "gcc-$GCC_VERSION"
  ./contrib/download_prerequisites
  ./configure --target=$TARGET --prefix="$PREFIX" --enable-languages=c --disable-nls --disable-werror --disable-multilib --with-newlib --disable-shared  --without-header --disable-plugin
  make all-gcc -j$(nproc)
  make install-gcc
  cd ..
  rm -fr "gcc-$GCC_VERSION" "gcc-$GCC_VERSION.tar.gz"
  cd $OSDEV_ROOT
}


function install_linker() {
  if [[ -f "$PREFIX/bin/$TARGET_LD" ]]; then
    return
  fi

  mkdir -p "$PREFIX/src"
  cd "$PREFIX/src"

  wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz

  tar xzvf binutils-$BINUTILS_VERSION.tar.gz

  cd "binutils-$BINUTILS_VERSION"
  ./configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
  make -j$(nproc)
  make install
  cd ..
  rm -fr "binutils-$BINUTILS_VERSION" "binutils-$BINUTILS_VERSION.tar.gz"
  cd $OSDEV_ROOT
}

function install_qemu() {
  if [[ -f "$PREFIX/bin/$TARGET_QEMU" ]]; then
    return
  fi

  mkdir -p "$PREFIX/src"
  cd "$PREFIX/src"

  wget https://download.qemu.org/qemu-$QEMU_VERSION.tar.xz

  tar xJvf qemu-$QEMU_VERSION.tar.xz

  cd "qemu-$QEMU_VERSION"
  ./configure --target-list="i386-softmmu" --prefix="$PREFIX"
  make -j$(nproc)
  make install
  cd ..
  rm -fr "qemu-$QEMU_VERSION" "qemu-$BINUTILS_VERSION.tar.xz"
  cd $OSDEV_ROOT
 
}

install_assembler
install_linker
install_compiler
install_qemu
