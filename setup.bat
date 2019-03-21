@ECHO OFF
SETLOCAL
PUSHD %~dp0
IF NOT EXIST Dependencies (
  mkdir Dependencies
)
PUSHD Dependencies

IF NOT EXIST Catch2-2.6.1 (
  git clone --branch v2.6.1 https://github.com/catchorg/Catch2.git Catch2-2.6.1
)

IF NOT EXIST pybind11-2.2.4 (
  git clone --branch v2.2.4 https://github.com/pybind/pybind11.git pybind11-2.2.4
)

IF NOT EXIST dlib-19.17 (
  wget https://github.com/davisking/dlib/archive/v19.17.zip -O dlib-v19.17.zip --no-check-certificate
  unzip dlib-v19.17.zip
  PUSHD dlib-19.17
  MKDIR Build
  MKDIR Install
  PUSHD Build
  cmake ../dlib -DCMAKE_INSTALL_PREFIX=../Install -DENABLE_ASSERTS=OFF
  cmake --build . --config Debug --target install
  cmake --build . --config Release --target install
  POPD
  POPD
)

POPD
POPD
ENDLOCAL
