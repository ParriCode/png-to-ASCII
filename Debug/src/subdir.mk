################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/csv.c \
../src/dataconfig.c \
../src/png_to_ASCII.c 

C_DEPS += \
./src/csv.d \
./src/dataconfig.d \
./src/png_to_ASCII.d 

OBJS += \
./src/csv.o \
./src/dataconfig.o \
./src/png_to_ASCII.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/csv.d ./src/csv.o ./src/dataconfig.d ./src/dataconfig.o ./src/png_to_ASCII.d ./src/png_to_ASCII.o

.PHONY: clean-src

