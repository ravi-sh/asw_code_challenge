#ifndef I2C_HEADER_H
#define I2C_HEADER_H

#include <stdint.h>

typedef enum {
    STATUS_OK,
    STATUS_ERROR
} status_t;

status_t i2c_read(
    uint8_t bus_address,
    uint8_t register_address,
    uint16_t length,
    uint8_t *buffer);

status_t i2c_write(
    uint8_t bus_address,
    uint8_t register_address,
    uint16_t length,
    uint8_t *buffer);

void get_fullscale_config( 
    unsigned char slave_addr, 
    unsigned char reg_addr);

void get_ODR(
    unsigned char slave_addr, 
    unsigned char reg_addr);
void set_ODR(
    unsigned char slave_addr, 
    unsigned char reg_addr, 
    unsigned char _data);

void en_int_pin(
    unsigned char slave_addr, 
    unsigned char reg_addr, 
    unsigned char _data);

void read_OD_axis(
    unsigned char slave_addr, 
    unsigned char reg_addr, 
    unsigned char axis );

void start();
void stop();
void ack();
void noack();

void write_byte(uint8_t c);
uint8_t read_byte(void);


#endif
