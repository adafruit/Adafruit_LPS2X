/*!
 *  @file Adafruit_LPS2X.h
 *
 * 	I2C Driver for the Library for the LPS2X family of barometric pressure
 *sensors
 *
 * 	This is a library for the Adafruit LPS2X breakout:
 * 	https://www.adafruit.com/products/4530
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
#define LPS2X_CTRL_REG3 0x22   ///< Third control register. Includes interrupt polarity
#define LPS2X_CTRL_REG4 0x23   ///< Fourth control register. Includes DRDY INT control
#define LPS2X_INTERRUPT_CFG 0x24   ///< Interrupt control register
#define LPS2X_PRESS_OUT_XL                                                     \
  (0x28 | 0x80) ///< | 0x80 to set auto increment on multi-byte read
#define LPS2X_TEMP_OUT_L (0x2B | 0x80) ///< | 0x80 to set auto increment on

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

class Adafruit_LPS2X;

/** Adafruit Unified Sensor interface for temperature component of LPS2X */
class Adafruit_LPS2X_Temp : public Adafruit_Sensor {
public:
  /** @brief Create an Adafruit_Sensor compatible object for the temp sensor
      @param parent A pointer to the LPS2X class */
  Adafruit_LPS2X_Temp(Adafruit_LPS2X *parent) { _theLPS2X = parent; }
  bool getEvent(sensors_event_t *);
  void getSensor(sensor_t *);

private:
  int _sensorID = 0x25;
  Adafruit_LPS2X *_theLPS2X = NULL;
};

/** Adafruit Unified Sensor interface for the pressure sensor component of LPS2X
 */
class Adafruit_LPS2X_Pressure : public Adafruit_Sensor {
public:
  /** @brief Create an Adafruit_Sensor compatible object for the pressure sensor
      @param parent A pointer to the LPS2X class */
  Adafruit_LPS2X_Pressure(Adafruit_LPS2X *parent) { _theLPS2X = parent; }
  bool getEvent(sensors_event_t *);
  void getSensor(sensor_t *);

private:
  int _sensorID = 0x26;
  Adafruit_LPS2X *_theLPS2X = NULL;
};

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the LPS2X I2C Barometric Pressure & Temperature Sensor
 */
class Adafruit_LPS2X {
public:
  Adafruit_LPS2X();
  ~Adafruit_LPS2X();

  bool begin_I2C(uint8_t i2c_addr = LPS2X_I2CADDR_DEFAULT,
                 TwoWire *wire = &Wire, int32_t sensor_id = 0);

  bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI,
                 int32_t sensor_id = 0);
  bool begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
                 int8_t mosi_pin, int32_t sensor_id = 0);
  void powerDown(bool power_down);
  lps2x_rate_t getDataRate(void);

  void setDataRate(lps2x_rate_t data_rate);
  void enableDataReadyInterrupts(bool enabled);
  void interruptsActiveLow(bool active_low);
  bool getEvent(sensors_event_t *pressure, sensors_event_t *temp);
  void reset(void);

  Adafruit_Sensor *getTemperatureSensor(void);
  Adafruit_Sensor *getPressureSensor(void);

protected:
  void _read(void);
  virtual bool _init(int32_t sensor_id);

  float unscaled_temp,   ///< Last reading's temperature (C) before scaling
      unscaled_pressure; ///< Last reading's pressure (hPa) before scaling

  uint16_t _sensorid_pressure, ///< ID number for pressure
      _sensorid_temp;          ///< ID number for temperature

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  Adafruit_SPIDevice *spi_dev = NULL; ///< Pointer to SPI bus interface

  Adafruit_LPS2X_Temp *temp_sensor = NULL; ///< Temp sensor data object
  Adafruit_LPS2X_Pressure *pressure_sensor =
      NULL; ///< Pressure sensor data object

private:
  friend class Adafruit_LPS2X_Temp;     ///< Gives access to private members to
                                        ///< Temp data object
  friend class Adafruit_LPS2X_Pressure; ///< Gives access to private
                                        ///< members to Pressure data
                                        ///< object

  void fillPressureEvent(sensors_event_t *pressure, uint32_t timestamp);
  void fillTempEvent(sensors_event_t *temp, uint32_t timestamp);
};

#endif