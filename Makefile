#makefile for BMP 280

#CC for cortex M4 with hard floating point unit 
CC= arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -c -g -std=c99 -DDEBUG --specs=nosys.specs
LK= arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -std=c99 -DDEBUG --specs=nosys.specs
all: bmp280_test

bmp280_test: main.o TivaC_GEN.o TivaC_I2C.o
	$(LK) main.o TivaC_GEN.o TivaC_I2C.o -o bmp280_test 

main.o: TivaC_GEN.h TivaC_I2C.h main.c 
	$(CC) main.c -o main.o

TivaC_GEN.o: TivaC_GEN.h tm4c123gh6pm.h TivaC_GEN.c 
	$(CC)  TivaC_GEN.c -o TivaC_GEN.o 

TivaC_I2C.o: TivaC_I2C.h TivaC_I2C.c tm4c123gh6pm.h 
	$(CC)  TivaC_I2C.c -o TivaC_I2C.o 

clean:
	rm -f *.o
	rm -f bmp280_test