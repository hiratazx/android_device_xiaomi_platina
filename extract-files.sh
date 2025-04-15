#!/bin/bash
#
# SPDX-FileCopyrightText: 2016 The CyanogenMod Project
# SPDX-FileCopyrightText: 2017-2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

function blob_fixup() {
    case "${1}" in
        vendor/lib64/hw/fingerprint.fpc.so)
            [ "$2" = "" ] && return 0
           "${PATCHELF}" --set-soname "fingerprint.fpc.so" "fingerprint.fpc.default.so" "${2}"
            ;;
        vendor/lib64/hw/fingerprint.goodix.so)
            [ "$2" = "" ] && return 0
           "${PATCHELF}" --set-soname "fingerprint.goodix.so" "fingerprint.goodix.default.so" "${2}"
            ;;
        *)
            return 1
            ;;
    esac

    return 0
}

function blob_fixup_dry() {
    blob_fixup "$1" ""
}

# If we're being sourced by the common script that we called,
# stop right here. No need to go down the rabbit hole.
if [ "${BASH_SOURCE[0]}" != "${0}" ]; then
    return
fi

set -e

export DEVICE=platina
export DEVICE_COMMON=sdm660-common
export VENDOR=xiaomi
export VENDOR_COMMON=${VENDOR}

"./../../${VENDOR_COMMON}/${DEVICE_COMMON}/extract-files.sh" "$@"
