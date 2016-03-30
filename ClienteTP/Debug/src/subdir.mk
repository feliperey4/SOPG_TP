################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ClienteTP.c \
../src/clte_admin.c \
../src/clte_listas.c \
../src/clte_sem.c \
../src/clte_shared.c \
../src/clte_sockets.c 

OBJS += \
./src/ClienteTP.o \
./src/clte_admin.o \
./src/clte_listas.o \
./src/clte_sem.o \
./src/clte_shared.o \
./src/clte_sockets.o 

C_DEPS += \
./src/ClienteTP.d \
./src/clte_admin.d \
./src/clte_listas.d \
./src/clte_sem.d \
./src/clte_shared.d \
./src/clte_sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


