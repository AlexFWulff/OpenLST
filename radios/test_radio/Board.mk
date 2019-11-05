RADIOS += test_radio
BOOTLOADERS += test_radio
test_radio_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# Source files                                                                                                                                                                    
test_radio_SRCS := \
        $(test_radio_DIR)/board.c

test_radio_CFLAGS := -DCUSTOM_BOARD_INIT -I$(test_radio_DIR)

# Disable UART0 in the bootloader to save space

test_radio_BOOTLOADER_CFLAGS := -DUART0_ENABLED=0
