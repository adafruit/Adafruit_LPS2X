
/*!
 *  @file Adafruit_LPS2X.cpp
 *
 *  @mainpage Library for the LPS2X family of barometric pressure sensors
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the LPS2X family of barometric pressure
 * sensors
 *
 * 	This is a library for the Adafruit LPS2X breakout:
 * 	https://www.adafruit.com/product/45XX
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_LPS2X.h"

/**
 * @brief Construct a new Adafruit_LPS2X::Adafruit_LPS2X object
 *
 */
Adafruit_LPS2X::Adafruit_LPS2X(void) {}
/**
 * @brief Destroy the Adafruit_LPS2X::Adafruit_LPS2X object
 *
 */
Adafruit_LPS2X::~Adafruit_LPS2X(void) { Serial.println("Called destructor"); }

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_LPS2X::begin_I2C(uint8_t i2c_address, TwoWire *wire,
                               int32_t sensor_id) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}

/*!  @brief Initializer for post i2c/spi init
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_LPS2X::_init(int32_t sensor_id) {

  Adafruit_BusIO_Register chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_WHOAMI, 1);

  // make sure we're talking to the right chip
  if (chip_id.read() != LPS2X_CHIP_ID) {
    return false;
  }
  _sensorid_pressure = sensor_id;
  _sensorid_temp = sensor_id + 1;

  reset();
  // do any software reset or other initial setup
  powerDown(false);
  setDataRate(LPS2X_RATE_25_HZ);

  // pressure_sensor = new Adafruit_LPS2X_Pressure(this);
  // temp_sensor = new Adafruit_LPS2X_Temp(this);

  return true;
}

/**
 * @brief Performs a software reset initializing registers to their power on
 * state
 *
 */
void Adafruit_LPS2X::reset(void) {
  Adafruit_BusIO_Register ctrl_2 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_CTRL_REG2, 1);
  Adafruit_BusIO_RegisterBits sw_reset =
      Adafruit_BusIO_RegisterBits(&ctrl_2, 1, 2);

  sw_reset.write(1);
  while (sw_reset.read()) {
    delay(1);
  }
}

/**
 * @brief Puts the sensor into power down mode, shutting the sensor down
 *
 * @param power_down
 */
void Adafruit_LPS2X::powerDown(bool power_down) {
  Adafruit_BusIO_Register ctrl_1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_CTRL_REG1, 1);
  Adafruit_BusIO_RegisterBits pd = Adafruit_BusIO_RegisterBits(&ctrl_1, 1, 7);
  pd.write(!power_down); // pd bit->0 == power down
}

/**
 * @brief Gets the current rate at which pressure and temperature measurements
 * are taken
 *
 * @return lps2x_rate_t The current data rate
 */
lps2x_rate_t Adafruit_LPS2X::getDataRate(void) {
  Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_CTRL_REG1, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl1, 3, 4);

  return (lps2x_rate_t)data_rate.read();
}
/**
 * @brief Sets the rate at which pressure and temperature measurements
 *
 * @param new_data_rate The data rate to set. Must be a `lps2x_rate_t`
 */
void Adafruit_LPS2X::setDataRate(lps2x_rate_t new_data_rate) {
  Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_CTRL_REG1, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl1, 3, 4);

  data_rate.write((uint8_t)new_data_rate);
}

/******************* Adafruit_Sensor functions *****************/
/*!
 *     @brief  Updates the measurement data for all sensors simultaneously
 */
/**************************************************************************/
void Adafruit_LPS2X::_read(void) {
  // get raw readings
  Adafruit_BusIO_Register pressure_data = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_TEMP_OUT_L, 2);
  Adafruit_BusIO_Register temp_data = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LPS2X_TEMP_OUT_L, 2);

  uint8_t buffer[2];
  temp_data.read(buffer, 2);

  int16_t raw_temp = buffer[1] << 8 | buffer[0];

  if (raw_temp & 0x8000) {
    raw_temp = raw_temp - 0xFFFF;
  }
  unscaled_temp = raw_temp;

  // rawAccX = buffer[9] << 8 | buffer[8];
  // rawAccY = buffer[11] << 8 | buffer[10];
  // rawAccZ = buffer[13] << 8 | buffer[12];

  // accX = rawAccX * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
  // accY = rawAccY * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
  // accZ = rawAccZ * accel_scale * SENSORS_GRAVITY_STANDARD / 1000;
}

/**************************************************************************/
/*!
    @brief  Gets the pressure sensor and temperature values as sensor events
    @param  pressure Sensor event object that will be populated with pressure
   data
    @param  temp Sensor event object that will be populated with temp data
    @returns True
*/
/**************************************************************************/
bool Adafruit_LPS2X::getEvent(sensors_event_t *pressure,
                              sensors_event_t *temp) {
  uint32_t t = millis();
  _read();

  // use helpers to fill in the events
  // fillPressureEvent(pressure, t);
  fillTempEvent(temp, t);
  return true;
}

void Adafruit_LPS2X::fillPressureEvent(sensors_event_t *pressure,
                                       uint32_t timestamp) {
  memset(pressure, 0, sizeof(sensors_event_t));
  pressure->version = sizeof(sensors_event_t);
  pressure->sensor_id = _sensorid_pressure;
  pressure->type = SENSOR_TYPE_PRESSURE;
  pressure->timestamp = timestamp;
  pressure->temperature = (unscaled_pressure / 4096.0);
}

void Adafruit_LPS2X::fillTempEvent(sensors_event_t *temp, uint32_t timestamp) {
  memset(temp, 0, sizeof(sensors_event_t));
  temp->version = sizeof(sensors_event_t);
  temp->sensor_id = _sensorid_temp;
  temp->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  temp->timestamp = timestamp;
  temp->temperature = (unscaled_temp / 480) + 42.5;
}