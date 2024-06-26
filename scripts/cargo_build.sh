#!/bin/bash
#
# Copyright 2024 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# A script that runs the portions of continuous integration that use cargo,
# including building and testing all targets in the workspace. If `release` is
# passed, also builds the binaries in release mode and exports them to
# BINARY_OUTPUTS_DIR.
set -e

cd $(dirname -- "$0")/..

source scripts/cargo_common.sh

# List of packages that will be built in release mode, along with the name of
# the resulting artifacts in BINARY_OUTPUTS_DIR.
declare -Ar RELEASE_PACKAGES=(
  [ledger_enclave_app]=ledger/binary
  [square_enclave_app]=square_example/binary
  [sum_enclave_app]=sum_example/binary
)

build_docker_image

docker run "${DOCKER_RUN_FLAGS[@]}" "${DOCKER_IMAGE_NAME}" sh -c 'cargo build && cargo test'

if [ "$1" == "release" ]; then
  args=()
  for pkg in "${!RELEASE_PACKAGES[@]}"; do args+=(-p "${pkg}"); done
  docker run "${DOCKER_RUN_FLAGS[@]}" "${DOCKER_IMAGE_NAME}" \
      cargo build --release "${args[@]}"

  # BINARY_OUTPUTS_DIR may be unset if this script is run manually; it'll
  # always be set during CI builds.
  if [[ -n "${BINARY_OUTPUTS_DIR}" ]]; then
    for pkg in "${!RELEASE_PACKAGES[@]}"; do
      src="target/x86_64-unknown-none/release/${pkg}"
      dst="${BINARY_OUTPUTS_DIR}/${RELEASE_PACKAGES[$pkg]}"
      mkdir --parents "$(dirname "${dst}")"
      cp "${src}" "${dst}"
    done
  fi
fi
