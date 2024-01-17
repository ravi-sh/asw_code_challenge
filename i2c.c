#include "i2c.h"
#include <stdint.h>
#include <stdio.h>

/* Asumption */

/*
 * I2C PIN/PORT CONFIGURATION NEEDS TODO
 * scl =
 * sda =
 * cs = 1;
 * The address is 0011100b, As the SDO/SA1 pin is connected to GND
 * SDO/SA1 = GND;
 * sa = 0011100b;
 *
 * */

/* Control registers */
# define CTRL_REG1	0x20
# define CTRL_REG2	0x21 /* FULL SCALE CONFIG BIN_VAL = 0010 0001 */
# define CTRL_REG3	0x22
# define CTRL_REG4	0x23
# define CTRL_REG5	0x24

# define INT_CFG 0x30

/* AXIS DATA REG */
# define OUT_X_L 0x28
# define OUT_X_H 0x29

# define OUT_Y_L 0x2A
# define OUT_Y_H 0x2B

# define OUT_Z_L 0x2C
# define OUT_Z_H 0x2D

/* Get the full-scale configuration
 *
 * slave addr = 0x0011100b
 * reg_addr = CTRL_REG2
 *
 * */
void get_fullscale_config( unsigned char slave_addr, unsigned char reg_addr) {
	uint8_t c;
	c = i2c_read(slave_addr, reg_addr, 1 /* byte*/, &c);
}

/* Get and set the output data rate
 * slave addr = 0x0011100b
 * reg_addr = CTRL_REG1
 * output data rates:
 *
 * */
void get_ODR(unsigned char slave_addr, unsigned char reg_addr) {
	uint8_t c;
	i2c_read(slave_addr, reg_addr, 1 /* bytes to read */, &c);
}

/*
 * Data = {
 * // Temperature sensor is disabled
 * TEMP_EN	= 0;
 * // X and Y axes operative mode selection (00 -> Low power, 01-> Medium-performance mode, 10-> High-performance mode, 11->Ultrahigh-performance mode)
 * OM1		= 0;
 * OM0		= 0
 *
 * // 000-> 0.625Hz, 001-> 1.25Hz (Refer data sheet Table no-21), for now I am selecting 001 (1.25Hz)
 * DO2 	= 0
 * DO1		= 0
 * DO0		= 1
 *
 * // FAST_ODR enables data rates higher than 80 Hz, Not required as the data rate is 1.25Hz
 * FAST_ODR	= 0
 *
 * // Enables self-test
 * ST 		= 0
 * }
 *
 * */
void set_ODR(unsigned char slave_addr, unsigned char reg_addr, unsigned char _data) {
	i2c_write(slave_addr, reg_addr, 1 /*Bytes to write */, &_data /* Data to write = 0 00 001 0 0  */ );
}

/* Enable or disable the interrupt pin
 * XIEN = 1; // Keeping the default, Enables interrupt generation on the X-axis
 * YIEN = 1; // Keeping the default, Enables interrupt generation on the Y-axis
 * ZIEN = 1; // Keeping the default, Enables interrupt generation on the Z-axis
 * 0 // Default set to zero, Not used
 * 1 // Default set to one, Not used
 * IEA = 0; // Interrupt active configuration on INT. Default value: 0
 * LIR = 0; // Latch interrupt request. Default value: 0
 * IEN = 1; // Enables the interrupt on the INT pin. Default value: 0 (0: disabled; 1: enabled)
 *
 * */
void en_int_pin(unsigned char slave_addr, unsigned char reg_addr, unsigned char _data){
	uint8_t c;
	i2c_read(slave_addr, reg_addr, 1 /* bytes to read */, &c);

	if ( c & 0x01 != 1 ) { //ENABLE INT
		i2c_write(slave_addr, reg_addr, 1 /*Bytes to write */, &_data /* Data to write = 111 01 001  */ );
	}
	else { // DISABLE INT
		i2c_write(slave_addr, reg_addr, 1 /*Bytes to write */, &_data /* Data to write = 111 01 000  */ );
	}
}

/* Read the output data of a specified axis
 * reg_addr = 0x28, 0x29 // x-axis
 * reg_addr = 0x2A, 0x2B // y-axis
 * reg_addr = 0x2C, 0x2D // z-axis
 *
 * */
void read_OD_axis(unsigned char slave_addr, unsigned char reg_addr, unsigned char axis ) {
	uint8_t c[2];
	/* X_AXIS */
	i2c_read(slave_addr, reg_addr, 2 /* bytes to read */, &c);
}

/* i2c START condition */
void start() {
	/* May need to add delay */
	scl=1;
	sda=1;
	sda=0;
}

/* i2c STOP condition */
void stop() {
	/* May need to add delay */
	sda=0;
	scl=1;
	sda=1;
}

/* I2C WRITE */
void write_byte(uint8_t c) {
	for (size_t i = 0; i <= 7; i++) {
		scl = 0;
		sda = ( c & (0x80 >> i ) ) ? 1 : 0;
		scl = 1;
	}
}

/* I2C WRITE */
uint8_t read_byte(void) {
	for ( size_t i = 0, j = 0; i < 8; i++) {
		scl = 0;
		scl = 1;
		if( sda ) {
			j |= ( 0x80 >> i ); /* j = j | (0x80)*/
		}
	}
	return j;
}

/* I2C ACK SIGNAL */
void ack() {
	scl=0;
	sda=2;
	scl=1;
	while ( sda == 1 );
	scl=0;
}

/* I2C NO ACK SIGNAL*/
void noack() {
	scl=0;
	sda=1;
	scl=1;
	scl=0;
}

status_t i2c_read(
		uint8_t bus_address,
		uint8_t register_address,
		uint16_t length,
		uint8_t *buffer)
{
	printf(
			"read [%d] bytes from bus [%d] for register [%d]\n",
			length,
			bus_address,
			register_address);

	/* Setting the output to some arbitrary value */
	for (size_t i = 0; i < length; ++i) {
		buffer[i] = 0xff;
	}

	start(); // Start condition
	write_byte(bus_address|0); // bus_address = Slave addr
	ack();
	write_byte(register_address);
	ack();

	// restart-start
	start();
	write_byte(bus_address|1); // read/write bit=1, means read from slave
	ack();

	for ( size_t i = 0; i < length; i++ ) {
		buffer[i] = read_byte();
		mack();
	}
	stop();
	//delay if required

	return STATUS_OK;
}

status_t i2c_write(
		uint8_t bus_address,
		uint8_t register_address,
		uint16_t length,
		uint8_t *buffer)
{
	printf(
			"write [%d] bytes to bus [%d] for register [%d]\n\t",
			length,
			bus_address,
			register_address);

	for (size_t i = 0; i < length; ++i) {
		printf("%p", buffer);
	}
	printf("\n");

	start();
	write_byte(bus_address|0);
	ack();
	write_byte(register_address);
	ack();

	for (size_t i = 0; i < length; i++) {
		write_byte(buffer[i]);
		ack();
	}

	stop();
	//delay if required

	return STATUS_OK;
}

