/*!
 *  @file Adafruit_LPS2X.h
 *
 * 	I2C Driver for the Library for the LPS2X family of barometric pressure
 *sensors
 *
 * 	This is a library for the Adafruit LPS2X breakout:
 * 	https://www.adafruit.com/products/45XX
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_LPS2X_H
#define _ADAFRUIT_LPS2X_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define LPS2X_I2CADDR_DEFAULT 0x5D ///< LPS2X default i2c address
#define LPS2X_CHIP_ID 0xBD         ///< LPS2X default device id from WHOAMI

#define LPS2X_EXAMPLE_REG 0x00 ///< Example register
#define LPS2X_WHOAMI 0x0F      ///< Chip ID register
#define LPS2X_CTRL_REG1 0x20   ///< First control register. Includes BD & ODR
#define LPS2X_CTRL_REG2 0x21   ///< Second control register. Includes SW Reset
#define LPS2X_PRESS_OUT_XL                                                     \
  (0x28 | 0x80) ///< | 0x80 to set auto increment on multi-byte read
#define LPS2X_TEMP_OUT_L (0x2B | 0x80) ///< | 0x80 to set auto increment on
// #define LPS2X_PRESS_OUT_XL 0x28  ///< pressure data registers start
// #define LPS2X_TEMP_OUT_L 0x2B  ///< temperature data registers start

/**
 * @brief
 *
 * Allowed values for `setDataRate`.
 */
typedef enum {
  LPS2X_RATE_ONE_SHOT,
  LPS2X_RATE_1_HZ,
  LPS2X_RATE_7_HZ,
  LPS2X_RATE_12_5_HZ,
  LPS2X_RATE_25_HZ,
} lps2x_rate_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the LPS2X I2C Digital Potentiometer
 */
class Adafruit_LPS2X {
public:
  Adafruit_LPS2X();
  ~Adafruit_LPS2X();

  bool begin_I2C(uint8_t i2c_addr = LPS2X_I2CADDR_DEFAULT,
                 TwoWire *wire = &Wire, int32_t sensor_id = 0);

  // bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI, int32_t sensorID =
  // 0); bool begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
  void reset(void);

  void powerDown(bool power_down);
  lps2x_rate_t getDataRate(void);
  void setDataRate(lps2x_rate_t data_rate);
  bool getEvent(sensors_event_t *pressure, sensors_event_t *temp);

protected:
  float unscaled_temp,   ///< Last reading's temperature (C) before scaling
      unscaled_pressure; ///< Last reading's pressure (hPa) before scaling
  uint16_t _sensorid_pressure, ///< ID number for pressure
      _sensorid_temp;          ///< ID number for temperature

  void _read(void);
  virtual bool _init(int32_t sensor_id);

private:
  Adafruit_I2CDevice *i2c_dev;
  Adafruit_SPIDevice *spi_dev;
  void fillPressureEvent(sensors_event_t *pressure, uint32_t timestamp);
  void fillTempEvent(sensors_event_t *temp, uint32_t timestamp);
};

#endif
