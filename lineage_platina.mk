#
# Copyright (C) 2018-2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit some common LineageOS stuff
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)
TARGET_BOOT_ANIMATION_RES := 1080
TARGET_SCREEN_WIDTH := 1080

# Inherit from platina device
$(call inherit-product, $(LOCAL_PATH)/device.mk)

PRODUCT_BRAND := Xiaomi
PRODUCT_DEVICE := platina
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_NAME := aosp_platina
PRODUCT_MODEL := Mi 8 Lite

# AxionAOSP flags
AXION_CAMERA_REAR_INFO := 12,5
AXION_CAMERA_FRONT_INFO := 5
AXION_MAINTAINER := ItzKaguya
AXION_PROCESSOR := Qualcomm_Snapdragon_660
BYPASS_CHARGE_SUPPORTED := true
AXION_CPU_SMALL_CORES := 0,1,2,3
AXION_CPU_BIG_CORES := 4,5,6,7
AXION_CPU_BG := 0-1
AXION_CPU_FG := 0-7

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="platina-user 10 QKQ1.190910.002 V12.0.3.0.QDTMIXM release-keys" \
    BuildFingerprint=Xiaomi/platina/platina:10/QKQ1.190910.002/V12.0.3.0.QDTMIXM:user/release-keys \
    DeviceProduct=platina
