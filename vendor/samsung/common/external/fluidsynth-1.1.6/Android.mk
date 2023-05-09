#
# fluidsynth-1.1.6
#

BUILD_ROOT_PATH := $(call my-dir)

FLUID_ROOT := $(call my-dir)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#ALSA_INCLUDES := \
#    vendor/samsung/variant/alsa-lib/include \
#    $(JACK_ROOT)/linux/alsa
#ALSA_INCLUDES := \
#    device/samsung/smdk_common/alsa-lib/include
#    $(JACK_ROOT)/linux/alsa
#include $(BUILD_ROOT_PATH)/src/bindings/Android.mk
#include $(BUILD_ROOT_PATH)/src/drivers/Android.mk for the future
#include $(BUILD_ROOT_PATH)/src/midi/Android.mk
#include $(BUILD_ROOT_PATH)/src/synth/Android.mk
#include $(BUILD_ROOT_PATH)/src/rvoice/Android.mk
#include $(BUILD_ROOT_PATH)/src/sfloader/Android.mk
#include $(BUILD_ROOT_PATH)/src/utils/Android.mk

fluid_include := \
	$(FLUID_ROOT)/src \
	$(FLUID_ROOT)/src/utils \
	$(FLUID_ROOT)/src/sfloader \
	$(FLUID_ROOT)/src/rvoice \
	$(FLUID_ROOT)/src/synth \
	$(FLUID_ROOT)/src/midi \
	$(FLUID_ROOT)/src/drivers \
	$(FLUID_ROOT)/src/bindings \
	$(FLUID_ROOT)/src/android \
	$(FLUID_ROOT)/bindings/fluidmax/fluidsynth \
	$(FLUID_ROOT)/include \
	$(FLUID_ROOT)/include/fluidsynth \
	$(FLUID_ROOT)/include/extra \
	$(FLUID_ROOT)/include/glib \
	$(FLUID_ROOT)/src/liblxdsp \
	$(FLUID_ROOT)/../../../../bionic/libc/include \
	$(FLUID_ROOT)/../../../../bionic/libm/include \
	$(FLUID_ROOT)/../../../../bionic/libm/include \


fluid_libsources := \
	src/utils/fluid_conv.c \
	src/utils/fluid_hash.c \
	src/utils/fluid_list.c \
	src/utils/fluid_ringbuffer.c \
	src/utils/fluid_settings.c \
	src/utils/fluid_sys.c \
	src/utils/fluid_mathfunc.c \
	src/sfloader/fluid_defsfont.c \
	src/sfloader/fluid_ramsfont.c \
	src/rvoice/fluid_adsr_env.c \
	src/rvoice/fluid_chorus.c \
	src/rvoice/fluid_iir_filter.c \
	src/rvoice/fluid_lfo.c \
	src/rvoice/fluid_rvoice.c \
	src/rvoice/fluid_rvoice_dsp.c \
	src/rvoice/fluid_rvoice_event.c \
	src/rvoice/fluid_rvoice_mixer.c \
	src/rvoice/fluid_rev.c \
	src/synth/fluid_chan.c \
	src/synth/fluid_event.c \
	src/synth/fluid_gen.c \
	src/synth/fluid_mod.c \
	src/synth/fluid_synth.c \
	src/synth/fluid_tuning.c \
	src/synth/fluid_voice.c \
	src/midi/fluid_midi.c \
	src/midi/fluid_midi_router.c \
	src/midi/fluid_seqbind.c \
	src/midi/fluid_seq.c \
	src/drivers/fluid_adriver.c \
	src/drivers/fluid_mdriver.c \
	src/drivers/fluid_aufile.c \
	src/bindings/fluid_cmd.c \
	src/bindings/fluid_ladspa.c \
	src/bindings/fluid_filerenderer.c \
	src/android/JAMSynth.c	


# ========================================================
# libfluidsynth.so
# ========================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(fluid_libsources)

#to avoid __cxa_XXXX errors
LOCAL_C_INCLUDES := $(fluid_include)
LOCAL_CFLAGS := -DHAVE_CONFIG_H
LOCAL_LDFLAGS := $(common_ldflags)
LOCAL_SHARED_LIBRARIES := libc libcutils libutils libstlport libdl libbinder libglib-2.0 libgthread-2.0
LOCAL_MODULE_TAGS := eng optional
LOCAL_MODULE := libfluidsynth

include $(BUILD_SHARED_LIBRARY)
