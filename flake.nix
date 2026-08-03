{
  description = "Nodery-OS Nix flake";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";

  outputs =
    { self, nixpkgs }:
    let
      pkgs = import nixpkgs { inherit system; };

      system = "x86_64-linux";
      target = "i386-elf";

      binutilsCross = import ./nix/binutils.nix { inherit pkgs target; };
      gccCross = import ./nix/gcc.nix { inherit pkgs target binutilsCross; };

      nodery-os = pkgs.stdenv.mkDerivation {
        pname = "nodery-os";
        version = "0";

        src = ./.;

        nativeBuildInputs = [
          binutilsCross
          gccCross
          pkgs.gnumake
          pkgs.nasm
        ];

        configurePhase = ''
          dd if=/dev/zero of=part.bin bs=1k count=32768
          ${pkgs.dosfstools}/bin/mkfs.fat -F16 part.bin
        '';

        buildPhase = ''
          make
        '';

        installPhase = ''
          mkdir -p $out
          cp os.bin $out/
        '';
      };

      run-os = pkgs.writeShellScriptBin "run-os" ''
        ${pkgs.qemu}/bin/qemu-system-i386 -m 512M -hda ${nodery-os}/os.bin -snapshot
      '';

    in
    {
      formatter.${system} = pkgs.nixfmt-tree;
      packages.${system}.default = nodery-os;
      apps.${system}.default = {
        type = "app";
        program = "${run-os}/bin/run-os";
      };

      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          binutilsCross
          gccCross
          pkgs.gnumake
          pkgs.nasm
          pkgs.qemu
          pkgs.gdb
        ];
      };
    };
}
