{
  pkgs, 
  target, 
  binutilsCross
}:

pkgs.stdenv.mkDerivation (finalAttrs: {
  pname = "gcc-cross-${target}";
  version = "14.2.0";
  
  src = pkgs.fetchurl {
    url = "https://mirror.ibcp.fr/pub/gnu/gcc-${finalAttrs.version}/gcc-${finalAttrs.version}.tar.xz";
    sha256 = "sha256-p7Obxpy/niWCbFpgqyZHcAH3wI2FzsBLwOKcq+1vPMk=";
  };

  nativeBuildInputs = [ pkgs.gmp pkgs.mpfr pkgs.libmpc pkgs.isl ];
  buildInputs = [ binutilsCross ];

  hardeningDisable = [ "all" ];
  enableParallelBuilding = true;

  preConfigure = ''
    mkdir build
    cd build
  '';

  configureScript = "../configure";

  configureFlags = [
    "--target=${target}"
    "--prefix=${placeholder "out"}"
    "--enable-languages=c"
    "--disable-bootstrap"
    "--disable-gcov"
    "--disable-libatomic"
    "--disable-libcc1"
    "--disable-libitm"
    "--disable-libgomp"
    "--disable-libmudflap"
    "--disable-libquadmath"
    "--disable-libmudflap"
    "--disable-libsanitizer"
    "--disable-libssp"
    "--disable-libstdcxx"
    "--disable-libstdcxx-pch"
    "--disable-libstdcxx-verbose"
    "--disable-multilib"
    "--disable-nls"
    "--disable-shared"
    "--disable-threads"
    "--without-headers"
    "--with-newlib"
    "--with-as=${pkgs.lib.getExe' binutilsCross "i386-elf-as"}"
    "--with-ld=${pkgs.lib.getExe' binutilsCross "i386-elf-ld"}"
  ];

  makeFlags = [
    "all-gcc"
    "all-c++tools"
    "all-target-libgcc"
  ];

  installFlags = [
    "install-gcc"
    "install-c++tools"
    "install-target-libgcc"
  ];
})

