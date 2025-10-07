{ 
  pkgs, 
  target 
}:

pkgs.stdenv.mkDerivation (finalAttrs: {
  pname = "binutils-cross-${target}";
  version = "2.43";

  src = pkgs.fetchurl {
    url = "https://mirror.ibcp.fr/pub/gnu/binutils/binutils-${finalAttrs.version}.tar.xz";
    sha256 = "sha256-tTYG9EOsjwHR1fycOUl/KvMi2Z4UzqXAtLEk1jA3k2U=";
  };

  enableParallelBuilding = true;
  hardeningDisable = ["all"];

  configureFlags = [
    "--target=${target}"
    "--prefix=${placeholder "out"}"
    "--disable-nls"
    "--disable-werror"
    "--disable-gold"
    "--disable-plugins"
    "--disable-multilib"
    "--disable-gprofng"
    "--disable-libquadmath"
    "--disable-libstdcxx"
    "--disable-readline"
    "--disable-sim"
  ];
})
