################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FileSysOSADA/osada.c 

OBJS += \
./src/FileSysOSADA/osada.o 

C_DEPS += \
./src/FileSysOSADA/osada.d 


# Each subdirectory must supply rules for building sources it contributes
src/FileSysOSADA/%.o: ../src/FileSysOSADA/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


