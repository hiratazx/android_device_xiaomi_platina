#
# Copyright (C) 2018-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from sdm660-common
include device/xiaomi/sdm660-common/BoardConfigCommon.mk

DEVICE_PATH := device/xiaomi/platina

# Assert
TARGET_OTA_ASSERT_DEVICE := platina

# Boot animation
TARGET_SCREEN_HEIGHT := 2280
TARGET_SCREEN_WIDTH := 1080

# Camera
BOARD_QTI_CAMERA_32BIT_ONLY := true
USE_DEVICE_SPECIFIC_CAMERA := true
TARGET_SUPPORT_HAL1 := false

# Display
TARGET_SCREEN_DENSITY := 440

# DT2W
TARGET_TAP_TO_WAKE_NODE := "/sys/touchpanel/double_tap"

# Kernel
TARGET_KERNEL_CONFIG := platina_defconfig

# Partitions
BOARD_SUPER_PARTITION_SYSTEM_DEVICE_SIZE := 3221225472
BOARD_SUPER_PARTITION_VENDOR_DEVICE_SIZE := 3221225472
BOARD_SUPER_PARTITION_SIZE := $(shell expr $(BOARD_SUPER_PARTITION_SYSTEM_DEVICE_SIZE) + $(BOARD_SUPER_PARTITION_VENDOR_DEVICE_SIZE) + $(BOARD_SUPER_PARTITION_CUST_DEVICE_SIZE))
BOARD_XIAOMI_DYNAMIC_PARTITIONS_SIZE := $(shell expr $(BOARD_SUPER_PARTITION_SIZE) - 4194304 )

# Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# SELinux
BOARD_VENDOR_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/vendor

# VINTF
DEVICE_MANIFEST_FILE += $(DEVICE_PATH)/configs/manifest.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += $(DEVICE_PATH)/configs/framework_compatibility_matrix.xml

# WLAN MAC
WLAN_MAC_SYMLINK := true

# Inherit the proprietary files
include vendor/xiaomi/platina/BoardConfigVendor.mk
