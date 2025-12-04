# TI C28x Architecture Plugin for Binary Ninja
A Binary Ninja architecture plugin providing support for the Texas Instruments C28x family of instructions.

> Note: this architecture is under active development. At present, it only disassembles the standard instruction set.
> Future work is planned to expand disassembly to the extended instruction set and to implement LLIL lifting.

## Overview
This architecture plugin provides support for the disassembling of C28x instructions.

### Features
- Full disassembly support for the standard TI C28x instruction set
- Compatibility mode support
  - Supports `C27x` mode (`OBJMODE` bit = 0, `AMODE` bit = 0)
  - Supports `C2xLP` mode (`OBJMODE` bit = 1, `AMODE` bit = 1)
  - Supports `C28x` mode (`OBJMODE` bit = 1, `AMODE` bit = 0)

## Installation
### Binary Download
The easiest way to use this plugin is to download and install a pre-compiled release binary.
1. Dowload the shared library corresponding to your OS/arch from the latest [GitHub Release](https://github.com/idaholab/bn-tic28x-arch/releases)
2. Copy the downloaded binary to your Binary Ninja plugin directory (e.g. `~/.binarynina/plygins/`)
3. Start Binary Ninja

### Manual Build and Installation
#### Prerequisites
- Binary Ninja (minimum version: `5.1.8104`)
- CMake 3.15 or higher
- C++20 compatible compiler

#### Building from Source & Install
```bash
# Clone the binja API
git clone https://github.com/Vector35/binaryninja-api.git
cd binaryninja-api

# Set up CMake files
echo -e "\nadd_subdirectory(plugins)" >> CMakeLists.txt
echo -e "\nadd_subdirectory(tic28x)" >> plugins/CMakeLists.txt

# Download TI C28x architecture source
cd plugins
git clone https://github.com/idaholab/bn-tic28x-arch
cd ..

# Build
cmake -DCMAKE_BUILD_TYPE=release -DHEADLESS=yes .
cmake --build . --target all -j

# Install
cp out/bin/libtic28x.so ~/.binaryninja/plugins/libtic28x.so
```

## Usage
### Opening Files
1. Open Binary Ninja and select "Open with Options..."
2. Under "Load Options", set your entry point offset and image base
3. Choose your desired compatibility mode from the "Platform" dropdown menu
    - Options are: `tic28x_c27x`, `tic28x_c2xlp`, and `tic28x`
4. Open the binary file and wait for auto-analysis to complete

> Note that you may need to manually define functions or customize your binary view for the file to load properly

## Development
### Building for Development
To build with debug symbols, follow the instructions above to build from source but change the build type to debug:
```bash
cmake -DCMAKE_BUILD_TYPE=debug -DHEADLESS=yes .
```

## Testing
This architecture supports testing for some instruction info and text generation. See the `*_test.cpp/h` files for details.
A test binary named `tic28x_architecture_test` is automatically created in the project directory when building. 
To run the tests, simply execute this binary. We welcome additional pull requests to further enhance this test suite!

### Contributing
Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a [Pull Request](https://github.com/idaholab/bn-tic28x-arch/pulls)

## Limitations
This plugin **does not** currently support the extended TI C28x instruction set.

This plugin **does not** currently lift any instructions to binja's LLIL.

While every effort was made to ensure the accuracy of this plugin, you may still encounter bugs such as:
- Improperly disassembled instructions
- Missing instructions
- Typos

If you encounter a bug, please consider opening an issue or pull request!

## Resources
- [Binary Ninja C++ API Documentation](https://api.binary.ninja/cpp/index.html)
- [TIC28x Instruction Set Reference](https://www.ti.com/lit/ug/spru430f/spru430f.pdf)
- [TIC28x Extended Instruction Set Reference](https://www.ti.com/lit/ug/spruhs1c/spruhs1c.pdf)

## License
Licensed under MIT.

See [LICENSE](LICENSE) file for details.

## Credits
Please see the [NOTICE](NOTICE.txt) file for details.

## Support
If you encounter issues with this repository, please create an [issue](https://github.com/idaholab/bn-st10-arch/issues).
