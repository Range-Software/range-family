# Range Software - Family
Family tree manager

## Help and tutorials

1. [General help](./src/family/help/general/en/index.md)
2. [Turorials](./src/family/help/tutorials/en/index.md)

## Prepare build environment
Initialize all submodules
```
git submodule init && git submodule update --remote
```
Following command will attempt to download and install all required packages, therefore it must be executed under priviledged (root) user
```
sudo ./src/range-build-tools/prereqs.sh
```
_NOTE: In case your OS does not provide you with Qt version 6.8 or newer download and install it from [https://www.qt.io/download/](https://www.qt.io/download/)._
## Build
```
cmake -S src -B build-Release && \
cmake --build build-Release --parallel
```
## Create an installation packages and installers
```
cmake --build build-Release --target package
```
## Installation using generated installers
### Windows

### MacOS
#### Drag & Drop
```
open './build-Release/range-family-1.0.0-darwin-arm64.dmg'
```
### Linux
#### Self extracting Tar GZip
```
./build-Release/range-family-1.0.0-linux-x86_64.sh
```
#### Debian DEB package
```
sudo apt install ./build-Release/range-family-1.0.0-linux-x86_64.deb
```
#### RedHat RPM package
```
sudo dnf install ./build-Release/range-family-1.0.0-linux-x86_64.rpm
```

## Download
To download already built binaries please visit http://range-software.com

## Powered by

* Qt - https://www.qt.io/
