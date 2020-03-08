# Put your stlink folder here so make burn will work.
STLINK=../../stlink

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=main

# Put your STM32x library code directory here
STM_COMMON=../../../

# Put your source files here (or *.c, etc)
# add startup file to build
SRCS = ./sdk/startup_stm32f030x6.s 
SRCS += system_stm32f0xx.c 
SRCS += main.c 
SRCS += delay.c 
SRCS += usart.c 
SRCS += rcc.c

OBJS = $(SRCS:.c=.o)


# Normally you shouldn't need to change anything below this line!
#######################################################################################
#GNUGCC = $(STM_COMMON)gcc-arm-none-eabi-4_9-2014q4/bin
GNUGCC = /bin
CC = $(GNUGCC)/arm-none-eabi-gcc
OBJCOPY = $(GNUGCC)/arm-none-eabi-objcopy
SIZE =  $(GNUGCC)/arm-none-eabi-size
LD = $(GNUGCC)/arm-none-eabi-ld 



CFLAGS  = -Os -Wfatal-errors -TSTM32F030F4P6.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0
CFLAGS  += -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS  += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS  += -fno-common -ffunction-sections -fdata-sections 
# CFLAGS += -flto # link time optimization
CFLAGS +=  -fno-exceptions



# important flag is -fsingle-precision-constant which prevents the double precision emulation
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 
CFLAGS += -I.
CFLAGS += -I./sdk


.PHONY: proj

all: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys -o $@ 
	$(CC) $(CFLAGS) -S $< $^
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(SIZE) -B  $(PROJ_NAME).elf
	ls -l $(PROJ_NAME).bin


clean:
	rm -rf *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin *.s
	ls

# Flash the STM32F4
upload: proj
	st-flash write $(PROJ_NAME).bin 0x08000000
	#../flash_write_counter	

hex2bin: proj
	$(OBJCOPY) -I ihex -O binary demo/demo.hex demo/demo.bin

read: proj
	$(STLINK)/st-flash read 0x40021000 0x00000010; xxd -p out.bin

erase: proj
	$(STLINK)/st-flash erase	
	
