{
  description = "Nodery-OS Nix shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";

  outputs = { self, nixpkgs }:
    let
      pkgs = import nixpkgs { inherit system; };
      
      system = "x86_64-linux";
      target = "i386-elf";

      binutilsCross = import ./nix/binutils.nix { inherit pkgs target; };
      gccCross = import ./nix/gcc.nix { inherit pkgs target binutilsCross; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          binutilsCross gccCross
          pkgs.gnumake pkgs.nasm pkgs.qemu pkgs.gdb
        ];
      };
    };
}
