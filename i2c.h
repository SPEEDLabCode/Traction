

#ifndef I2C_H
#define I2C_H
/** The \p ack_or_nack argument for i2c_byte().
    The values of this enum currently match the value we pass to ACKDT. */
enum i2c_ack {
  /** Don't acknowledge. */
  I2C_NACK = 1,
  /** Do acknowledge. */
  I2C_ACK = 0 
  };

void Init_I2C1(void);
void DisI2C1(void);
void i2c_start1(void);
void i2c_rstart1(void);
void i2c_stop1(void);
unsigned char  i2c_sbyte1(unsigned char);
unsigned char i2c_rbyte1(enum i2c_ack);

void Init_I2C2(void);
void DisI2C2(void);
void i2c_start2(void);
void i2c_rstart2(void);
void i2c_stop2(void);
unsigned char  i2c_sbyte2(unsigned char);
unsigned char i2c_rbyte2(enum i2c_ack);

void Init_I2C3(void);
void DisI2C3(void);
void i2c_start3(void);
void i2c_rstart3(void);
void i2c_stop3(void);
unsigned char  i2c_sbyte3(unsigned char);
unsigned char i2c_rbyte3(enum i2c_ack);


#endif


