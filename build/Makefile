SUB_DIRS=src utils/tinyaes
OBJS=\
	main.o \
	src/ac.o \
	src/bod.o \
	src/clkctrl.o \
	src/communication.o \
	src/control.o \
	src/cpuint.o \
	src/driver_init.o \
	src/protected_io.o \
	src/pwm_basic.o \
	src/rtc.o \
	src/slpctrl.o \
	src/spi_basic.o \
	src/tasks.o \
	src/tca.o \
	src/usart_basic.o \
	src/vref.o \
	src/wdt.o \
	utils/tinyaes/aes.o

BLOB_DIR=blob

BLOBS=\
	$(BLOB_DIR)/cipher.o

CIPHER=\
	$(BLOB_DIR)/cipher.bin

DEPS:=$(OBJS:%.o=%.d)

OUTPUT_FILE_NAME:=binary.elf

BASEDIR=..
DEVROOT=dev

MCU=attiny816
MCU_BASEARCH=avrxmega3

MKDIR=mkdir -p
RM=rm -f
RMDIR=rmdir

CC=avr-gcc
AES_CONFIG_FLAGS=-DCBC=1 -DECB=0 -DCTR=0
CFLAGS=-DDEBUG -Os -ffunction-sections -g3 -Wall -c -std=gnu99 -mmcu=$(MCU) -Wa,-mgcc-isr -B $(DEVROOT) -I$(BASEDIR)/config -I$(BASEDIR)/include -I$(BASEDIR)/utils $(AES_CONFIG_FLAGS) #-DF_CPU=10000000 -D__tiny816__
DEPSFLAGS=-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"

AS=avr-gcc -x assembler-with-cpp
ASFLAGS=-I$(BASEDIR)/assembler

LDFLAGS=-Wl,--start-group -lm -Wl,--end-group -Wl,-Map=$@.map -Wl,--gc-sections -mmcu=$(MCU) -B dev/$(MCU)

AES_KEYFILE=keys/communication.o

OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

SIZE=avr-size

vpath %.c ../
vpath %.s ../
vpath %.S ../

all: $(SUB_DIRS) $(BLOB_DIR) $(KEYS) $(OUTPUT_FILE_NAME)

$(OUTPUT_FILE_NAME): $(OBJS) $(BLOBS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(OBJCOPY) -O binary "$@" "$@.bin"
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $@ $@.hex
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary $@ $@.eep || exit 0
	$(OBJDUMP) -h -S $@ > $@.lss
	$(SIZE) $@

%.o: %.c
	$(CC) $(CFLAGS) $(DEPSFLAGS) -o "$@" "$<"

%.o: %.s
	$(AS) $(CFLAGS) $(DEPSFLAGS) -o "$@" "$<"

%.o: %.S
	$(AS) $(CFLAGS) $(DEPSFLAGS) -o "$@" "$<"

$(BLOB_DIR)/%.o: $(BLOB_DIR)/%.bin
	$(OBJCOPY) -I binary -O elf32-avr -B avr:103 \
		--rename-section .data=.progmem.data,contents,alloc,load,readonly,data \
		--redefine-sym _binary_blob_$*_bin_start=blob_$* \
		--redefine-sym _binary_blob_$*_bin_end=blob_$*_end \
		--redefine-sym _binary_blob_$*_bin_size=blob_$*_size \
		"$<" "$@"
#	sed "s:NAME:$$(basename "$<" .blob):g" $(BASEDIR)/utils/blob.template.s | sed 's:FILE:$<:g' | $(AS) $(CFLAGS) -o "$@" -

$(CIPHER):
	@if [ ! -r "$@" ]; then openssl rand 16 > "$@"; else echo "note: AES key exists, keeping it"; fi
	@echo -n "checksum: "; sha256sum "$@"

# Detect changes in the dependent files and recompile the respective object files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(SUB_DIRS):
	$(MKDIR) $@

.PHONY: clean

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) $(OUTPUT_FILE_NAME)
	$(RM) $(OUTPUT_FILE_NAME).a $(OUTPUT_FILE_NAME).hex $(OUTPUT_FILE_NAME).bin
	$(RM) $(OUTPUT_FILE_NAME).lss $(OUTPUT_FILE_NAME).eep $(OUTPUT_FILE_NAME).map $(OUTPUT_FILE_NAME).srec
	-$(RMDIR) $(SUB_DIRS)

.PHONY: purge

purge: clean
	$(RM) -f $(CIPHER) $(BLOBS)
