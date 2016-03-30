################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ServerTP.c \
../src/svr_admin.c \
../src/svr_clientes.c \
../src/svr_listas.c \
../src/svr_sem.c \
../src/svr_shared.c \
../src/svr_sockets.c 

OBJS += \
./src/ServerTP.o \
./src/svr_admin.o \
./src/svr_clientes.o \
./src/svr_listas.o \
./src/svr_sem.o \
./src/svr_shared.o \
./src/svr_sockets.o 

C_DEPS += \
./src/ServerTP.d \
./src/svr_admin.d \
./src/svr_clientes.d \
./src/svr_listas.d \
./src/svr_sem.d \
./src/svr_shared.d \
./src/svr_sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


