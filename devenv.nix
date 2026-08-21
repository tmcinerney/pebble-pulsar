{ pkgs, lib, config, inputs, ... }:

{
  # ARM GCC cross-compiler & build dependencies for Pebble C SDK
  packages = [
    pkgs.gcc-arm-embedded
    pkgs.freetype
  ];

  languages.python = {
    enable = true;
    venv = {
      enable = true;
      requirements = ''
        pebble-tool
      '';
    };
  };

  # Unset host CC/CXX in devenv shell so Pebble WAF uses arm-none-eabi-gcc for cross-compilation
  enterShell = ''
    unset CC
    unset CXX
    echo "Pebble SDK development environment loaded."
    echo "Ready to build! Run 'pebble build' or 'pebble install --emulator emery'."
  '';
}
