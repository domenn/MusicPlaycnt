#!/usr/bin/bash
_V=0
PLOG_VER=1.1.5

function up_until() {
  if [[ $_V -eq 1 ]]; then
    echo iteration :....
  fi
  my_pwd=$2
  if [[ $_V -eq 1 ]]; then
    echo my_pwd $my_pwd
  fi

  r_part1=${my_pwd%%+(/)}
  if [[ $_V -eq 1 ]]; then
    echo r_part1 $r_part1
  fi
  result=${r_part1##*/}
  if [[ $_V -eq 1 ]]; then
    echo result $result
  fi

  if [ "$result" == "$1" ]; then
    return 0
  fi
  up_until "$1" "${r_part1%/*}"
}
shopt -s extglob
myself=$(readlink -f "$0")
up_until "scripts" "$myself"

# shellcheck disable=SC2164
pushd "$my_pwd"
mkdir -p ../ext/builds
# shellcheck disable=SC2164
cd ../ext/builds
# shellcheck disable=SC2046
curl -SsLO https://github.com/SergiusTheBest/plog/archive/${PLOG_VER}.zip && unzip -oq ${PLOG_VER}.zip && rm -rf ${PLOG_VER}.zip \
 && cd plog-${PLOG_VER}  \
 && cmake -G "MSYS Makefiles" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_MAKE_PROGRAM=mingw32-make -DPLOG_BUILD_SAMPLES:BOOL="0" ../plog-${PLOG_VER} -DPLOG_BUILD_SAMPLES:BOOL="0" -DCMAKE_INSTALL_PREFIX=../../installs -DCMAKE_BUILD_TYPE=Release . \
 && cmake --build . && cmake --build . --target install

# shellcheck disable=SC2164
popd