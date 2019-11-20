SUB_DIRS=src
OBJS=\
	main.o \
	src/ac.o \
	src/bod.o \
	src/clkctrl.o \
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
	src/wdt.o

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
CFLAGS=-DDEBUG -Os -ffunction-sections -g3 -Wall -c -std=gnu99 -mmcu=$(MCU) -Wa,-mgcc-isr -B $(DEVROOT) -I$(BASEDIR) -I$(BASEDIR)/config -I$(BASEDIR)/include -I$(BASEDIR)/utils #-DF_CPU=10000000 -D__tiny816__
DEPSFLAGS=-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"

AS=avr-gcc -x assembler-with-cpp
ASFLAGS=-I$(BASEDIR)/assembler

LDFLAGS=-Wl,--start-group -lm -Wl,--end-group -Wl,-Map=$@.map -Wl,--gc-sections -mmcu=$(MCU) -B dev/$(MCU)

OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

SIZE=avr-size

vpath %.c ../
vpath %.s ../
vpath %.S ../

all: $(SUB_DIRS) $(OUTPUT_FILE_NAME)

$(OUTPUT_FILE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
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

# Detect changes in the dependent files and recompile the respective object files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(SUB_DIRS):
	$(MKDIR) $@

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) $(OUTPUT_FILE_NAME)
	$(RM) $(OUTPUT_FILE_NAME).a $(OUTPUT_FILE_NAME).hex $(OUTPUT_FILE_NAME).bin
	$(RM) $(OUTPUT_FILE_NAME).lss $(OUTPUT_FILE_NAME).eep $(OUTPUT_FILE_NAME).map $(OUTPUT_FILE_NAME).srec
	$(RMDIR) $(SUB_DIRS)
