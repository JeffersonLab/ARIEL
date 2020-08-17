function find_cetmodules_dir()
{
  local latest_version_dir="$(ls -1d "${1}/cetmodules/"*.version | sort -V -r | head -1)"
  echo "${latest_version_dir%.version}"
}

[[ -n "${CETMODULES_DIR}" ]] && [[ -x checkClassVersion ]] && return
TOP_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd -P)
CETMODULES_DIR="$(find_cetmodules_dir "${TOP_DIR}")"
export CMAKE_PREFIX_PATH="${CETMODULES_DIR}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
export PATH="${CETMODULES_DIR}/bin:${PATH}"
