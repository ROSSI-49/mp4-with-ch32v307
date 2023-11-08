################################################################################
# MRS Version: 1.9.1
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/IIC.c \
../User/ch32v30x_it.c \
../User/diskio.c \
../User/es8388.c \
../User/ff.c \
../User/ffsystem.c \
../User/ffunicode.c \
../User/lcd.c \
../User/main.c \
../User/pwm.c \
../User/sdio.c \
../User/system_ch32v30x.c \
../User/videoplay.c 

OBJS += \
./User/IIC.o \
./User/ch32v30x_it.o \
./User/diskio.o \
./User/es8388.o \
./User/ff.o \
./User/ffsystem.o \
./User/ffunicode.o \
./User/lcd.o \
./User/main.o \
./User/pwm.o \
./User/sdio.o \
./User/system_ch32v30x.o \
./User/videoplay.o 

C_DEPS += \
./User/IIC.d \
./User/ch32v30x_it.d \
./User/diskio.d \
./User/es8388.d \
./User/ff.d \
./User/ffsystem.d \
./User/ffunicode.d \
./User/lcd.d \
./User/main.d \
./User/pwm.d \
./User/sdio.d \
./User/system_ch32v30x.d \
./User/videoplay.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"C:\Users\77249\Desktop\串口+按键可用+11fps\Debug" -I"C:\Users\77249\Desktop\串口+按键可用+11fps\Core" -I"C:\Users\77249\Desktop\串口+按键可用+11fps\User" -I"C:\Users\77249\Desktop\串口+按键可用+11fps\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

