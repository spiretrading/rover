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

POPD
POPD
ENDLOCAL
