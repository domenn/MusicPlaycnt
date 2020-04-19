#!/usr/bin/bash
# _V=1

YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function logv () {
    if [[ $_V -eq 1 ]]; then
        printf "${YELLOW}$*${NC}\n"
    fi
}


PF_DL_REF=54abe09d346a196337abcb1c6a4ec3b532037cc3
PF_DL_LINK=https://github.com/sago007/PlatformFolders/archive/${PF_DL_REF}.zip

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

INS_PREFIX=installs
EXTRA_CMAKES=
BUILD_DIR_SFX=nix
CMAKE_BUILD_CMD="--build . && cmake --build . --target install"
UNAME=$(uname)
if [[ "$UNAME" == CYGWIN* || "$UNAME" == MINGW* ]] ; then
  logv we have windows
	# Windows: If we have gcc, use it. Otherwise msvc.
	which gcc &>/dev/null; whichGccExitCode=$?
	logv rv $whichGccExitCode
	if [ "$whichGccExitCode" -eq "0" ] ; then
	  logv which_EQ0
    EXTRA_CMAKES=(-G "MSYS Makefiles" "-DCMAKE_CXX_COMPILER=g++" "-DCMAKE_MAKE_PROGRAM=mingw32-make")
    BUILD_DIR_SFX=mingw
  else
    logv which_EQ1
    INS_PREFIX=installs_msvc
    EXTRA_CMAKES=(-G "Visual Studio 16 2019" -A x64 "-DCMAKE_CONFIGURATION_TYPES:STRING=Debug;Release")
    CMAKE_BUILD_CMD="--build . --config Debug && cmake --build . --config Release && cmake --build . --config Debug --target install && cmake --build . --config Release --target install"
    BUILD_DIR_SFX=msvc
	fi
fi
logv EXTRA_CMAKES "${EXTRA_CMAKES[@]}"

# shellcheck disable=SC2164
pushd "$my_pwd"
mkdir -p ../ext/builds${BUILD_DIR_SFX}
# shellcheck disable=SC2164
cd ../ext/builds${BUILD_DIR_SFX}
# shellcheck disable=SC2046

logv WILL: cmake ${CMAKE_BUILD_CMD}
curl -SsLO $PF_DL_LINK && unzip -oq ${PF_DL_REF}.zip && rm -rf ${PF_DL_REF}.zip  \
 && \
cd PlatformFolders-${PF_DL_REF}  \
 && cmake "${EXTRA_CMAKES[@]}" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING:BOOL="0" -DCMAKE_INSTALL_PREFIX=../../${INS_PREFIX} . \
 && eval "cmake ${CMAKE_BUILD_CMD}"

# shellcheck disable=SC2164
popd

# read -p "Press enter to continue"