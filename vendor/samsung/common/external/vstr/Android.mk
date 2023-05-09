#
#  Copyright (C) 2013 Samsung Electronics Co. Ltd.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/fix.c \
	src/vstr_add.c \
	src/vstr_add_fmt.c \
	src/vstr_add_netstr.c \
	src/vstr.c \
	src/vstr_cache.c \
	src/vstr_cmp.c \
	src/vstr_cntl.c \
	src/vstr_conv.c \
	src/vstr_cstr.c \
	src/vstr_data.c \
	src/vstr_del.c \
	src/vstr_dup.c \
	src/vstr_export.c \
	src/vstr_fmt.c \
	src/vstr_inline.c \
	src/vstr_mov.c \
	src/vstr_parse.c \
	src/vstr_parse_netstr.c \
	src/vstr_ref.c \
	src/vstr_sc.c \
	src/vstr_sc_posix.c \
	src/vstr_sect.c \
	src/vstr_split.c \
	src/vstr_spn.c \
	src/vstr_srch.c \
	src/vstr_srch_case.c \
	src/vstr_sub.c \
	src/vstr_version.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES := 

LOCAL_SHARED_LIBRARIES := 

LOCAL_CFLAGS := -DUSE_RESTRICTED_HEADERS -DHAVE_MMAP -DHAVE_MMAP64 -DSTDC_HEADERS

LOCAL_CFLAGS += 

LOCAL_MODULE := libvstr

include $(BUILD_SHARED_LIBRARY)

