#
# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Base configuration for communication-oriented android devices
# (phones, tablets, etc.).  If you want a change to apply to ALL
# devices (including non-phones and non-tablets), modify
# core_minimal.mk instead.

PRODUCT_PACKAGES += \
    BasicDreams \
    Browser \
    Contacts \
    DocumentsUI \
    DownloadProviderUi \
    ExternalStorageProvider \
    KeyChain \
    PicoTts \
    PacProcessor \
    ProxyHandler \
    SharedStorageBackup \
    VpnDialogs
    
PRODUCT_PACKAGES += \
    e2fsck \
    blkid \
    libhyphenation \
    libkeyutils \
    libexifa \
    libjpega \
    libwebcore \
    ebtables    

# charon
    PRODUCT_PACKAGES += \
    charon \
    libcharon.so \
    libstrongswan.so \
    libvstr.so

# ProfessionalAudio
PRODUCT_PACKAGES += \
    libjackshm \
    libjackserver \
    libjack \
    androidshmservice \
    jackd \
    jack_dummy \
    jack_alsa \
    jack_goldfish \
    jack_opensles \
    jack_loopback \
    jack_connect \
    jack_disconnect \
    jack_lsp \
    jack_showtime \
    jack_simple_client \
    jack_transport \
    libasound \
    libglib-2.0 \
    libgthread-2.0 \
    libfluidsynth

$(call inherit-product, $(SRC_TARGET_DIR)/product/core_minimal.mk)
