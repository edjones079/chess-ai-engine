
{
  description = "A Nix-flake-based C/C++ development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable"; # stable Nixpkgs

  outputs =
    { self, ... }@inputs:

    let

	  system = "x86_64-linux";
      pkgs = import inputs.nixpkgs { inherit system; };

    in
    {
        devShells.${system}.default = pkgs.mkShell{

		buildInputs = with pkgs; [
            clang-tools
            cmake
            codespell
            conan
            cppcheck
            doxygen
		    gcc
			glew
		    glfw
            glibc
            gtest
            lcov
			libGL
	      	libGLU
		    mesa
			pkg-config
            vcpkg
            vcpkg-tool
            unzip
			xorg.libX11
	      	xorg.libXrandr
	      	xorg.libXi
	      	xorg.libXcursor
	      	xorg.libXinerama
            ]
            ++ (if system == "aarch64-darwin" then [ ] else [ gdb ]);
        };
    };
}