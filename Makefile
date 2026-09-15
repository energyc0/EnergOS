MCU = -mcpu=cortex-m4
LINKER_SCRIPT = energos.ld
CFLAGS = $(MCU) -mthumb \
	-O0 -g3 -Wall -Wextra\
	-c -T $(LINKER_SCRIPT) -ffreestanding -nostdlib

TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
LD = $(TOOLCHAIN)ld
FIRMWARE_NAME = energos
FIRMWARE_FILE = $(FIRMWARE_NAME).bin
ELF_FILE = $(FIRMWARE_NAME).elf

$(FIRMWARE_FILE): energos.c startup.c
	$(CC) $(CFLAGS) energos.c -o energos.o
	$(CC) $(CFLAGS) startup.c -o startup.o
	$(LD) -T $(LINKER_SCRIPT) energos.o startup.o -o $(ELF_FILE)
	$(TOOLCHAIN)objcopy -O binary $(ELF_FILE) $(FIRMWARE_FILE)

readelf: 
	$(TOOLCHAIN)readelf -a $(ELF_FILE)

objdump:
	$(TOOLCHAIN)objdump -D $(ELF_FILE) > $(FIRMWARE_NAME).asm

flash: $(FIRMWARE_FILE)
	openocd -f interface/stlink.cfg \
	-f target/stm32f4x.cfg -c "program $(ELF_FILE) verify reset exit"
#openocd -f interface/stlink.cfg \
#-f target/stm32f4x.cfg

clean:
	rm -f *.o *.elf *.bin

.PHONY: clean readelf flash