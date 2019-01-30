@ECHO OFF
SETLOCAL
PUSHD %~dp0
IF NOT EXIST Dependencies (
  mkdir Dependencies
)
PUSHD Dependencies

IF NOT EXIST Catch2 (
  git clone --branch v2.2.1 https://github.com/catchorg/Catch2.git Catch2
)

POPD
POPD
ENDLOCAL
