NAME := pcm_resampler

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

$(NAME)_INCLUDES := ./pcm_resampler

#pcm_resampler lib
$(NAME)_SOURCES +=  resampler.c \
		    pcm_resampler.c

