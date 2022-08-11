#!/bin/bash

# ./deploy.sh <VERSION> <REF> <PRIVATE_TOKEN>
# <VERSION>       Version you are deploying.
# <REF>           script gets the dist files from latest CI pipeline on this ref.
#                 Typically, a vX.Y.Z tag.
# <PRIVATE_TOKEN> Token used for uploading/deleting files to package registry using API.
VERSION=$1
REF=$2
PRIVATE_TOKEN=$3

[ ! -z "$VERSION" ] || exit 1
[ ! -z "$REF" ] || exit 1
[ ! -z "$PRIVATE_TOKEN" ] || exit 1

CURL_W_HEADER="curl --silent  --header \"PRIVATE-TOKEN: ${PRIVATE_TOKEN}\""
GITLAB_API="https://gitlab.com/api/v4"

PIPELINE_ID=`curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" "${GITLAB_API}/projects/nomadic-labs%2Fumami-wallet%2Fumami/pipelines" \
		  | jq "map(select(.ref == \"${REF}\") | .id)[0]"`

DIST_JOBS=`curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" "${GITLAB_API}/projects/nomadic-labs%2Fumami-wallet%2Fumami/pipelines/${PIPELINE_ID}/jobs" \
		| jq  'map(select(.stage == "dist"))'`

# get_job <NAME>
get_job () { echo ${DIST_JOBS} \
		 | jq "map(select(.name == \"$1\"))[0].id"; }

JOB_LINUX="$(get_job linux-dist)"
JOB_MACOS="$(get_job macos-dist)"
JOB_WINDOWS="$(get_job windows-dist)"

# get_artifacts <JOB_ID> <OUTPUT.ZIP>
get_artifacts () { curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" --location --output "$2" "${GITLAB_API}/projects/nomadic-labs%2Fumami-wallet%2Fumami/jobs/$1/artifacts"; }

ZIP_LINUX="linux.zip"
ZIP_MACOS="macos.zip"
ZIP_WINDOWS="windows.zip"

get_artifacts "${JOB_LINUX}" "${ZIP_LINUX}"
get_artifacts "${JOB_MACOS}" "${ZIP_MACOS}"
get_artifacts "${JOB_WINDOWS}" "${ZIP_WINDOWS}"

UPLOADS_DIR=umami_release_uploads

rm -rf "${UPLOADS_DIR}" && mkdir "${UPLOADS_DIR}"

FILE_LINUX_RPM="umami-${VERSION}.x86_64.rpm"
FILE_LINUX_DEB="umami_${VERSION}_amd64.deb"
FILE_MACOS_LATEST="latest-mac.yml"
FILE_MACOS_DMG="umami-${VERSION}.dmg"
FILE_MACOS_ZIP="umami-${VERSION}-mac.zip"
FILE_WINDOWS_LATEST="latest.yml"
FILE_WINDOWS_EXE_ORIGIN="umami Setup ${VERSION}.exe"
FILE_WINDOWS_EXE="umami.Setup.${VERSION}.exe"

# extract <SRC_ZIP> <FILENAME> [DST_FILENAME]
extract () {
    local DST=$([ -z "$3" ] && echo "$2" || echo "$3");
    unzip -p "$1" "dist/$2" > "${UPLOADS_DIR}/$DST";
}

extract "${ZIP_LINUX}" "${FILE_LINUX_RPM}"
extract "${ZIP_LINUX}" "${FILE_LINUX_DEB}"
extract "${ZIP_MACOS}" "${FILE_MACOS_LATEST}"
extract "${ZIP_MACOS}" "${FILE_MACOS_DMG}"
extract "${ZIP_MACOS}" "${FILE_MACOS_ZIP}"
extract "${ZIP_WINDOWS}" "${FILE_WINDOWS_LATEST}"
extract "${ZIP_WINDOWS}" "${FILE_WINDOWS_EXE_ORIGIN}" "${FILE_WINDOWS_EXE}"

PROJECT_PATH="nomadic-labs%2Fumami-wallet%2Fumami-package-registry-test" # FIXME: umami-package-registry-test -> umami
PROJECT_NAME="umami-package-registry-test"                               # FIXME: umami-package-registry-test -> umami
PACKAGE_ID="8541436"                                                     # FIXME: 8541436 -> 5720836

PACKAGE_FILES=`curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" "${GITLAB_API}/projects/${PROJECT_PATH}/packages/${PACKAGE_ID}/package_files/"`

function get_old_file_id () {
     echo "${PACKAGE_FILES}" | jq "map(select(.file_name == \"${1}\").id)[0]"
}

function delete () {
    curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" --request DELETE "${GITLAB_API}/projects/${PROJECT_PATH}/packages/${PACKAGE_ID}/package_files/${1}"
}

# upload <FILENAME>
function upload () {
    local OLD="$(get_old_file_id ${1})"
    curl --silent --header "PRIVATE-TOKEN: ${PRIVATE_TOKEN}" --upload-file "${UPLOADS_DIR}/${1}" "${GITLAB_API}/projects/${PROJECT_PATH}/packages/generic/${PROJECT_NAME}/update/${1}?status=default&select=package_file" | jq . ;
    [ 'null' == "${OLD}" ] || (echo "Deleting old version of ${1} (${OLD})" && delete "${OLD}")
}

upload "${FILE_LINUX_RPM}"
upload "${FILE_LINUX_DEB}"
upload "${FILE_MACOS_LATEST}"
upload "${FILE_MACOS_DMG}"
upload "${FILE_MACOS_ZIP}"
upload "${FILE_WINDOWS_LATEST}"
upload "${FILE_WINDOWS_EXE}"
