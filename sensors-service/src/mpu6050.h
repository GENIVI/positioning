/**************************************************************************
 * @brief Access library for MPU6050/MPU9150 inertial sensors 
 *
 * @details Encapsulate I2C access to a MPU6050 sensor on a Linux machine
 * The MPU6050 from Invense is a 6DOF inertial sensor
 * @see http://www.invensense.com/mems/gyro/mpu6050.html 
 * The functions work also with the MPU9150 which is a MPU6050 with 
 * additional functionality (magnetometer)
 * @see http://www.invensense.com/mems/gyro/mpu9150.html
 *
 *
 * @author Helmut Schmidt <https://github.com/huirad>
 * @copyright Copyright (C) 2015, Helmut Schmidt
 * 
 * @license MPL-2.0 <http://spdx.org/licenses/MPL-2.0>
 *
 **************************************************************************/

#ifndef INCLUDE_MPU6050
#define INCLUDE_MPU6050

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
 
 
/** Part 0: I2C device names and device addresses
 *
 */

/** Typical I2C device names on Linux machines
  * For early Raspberry Pi machines, its typically "/dev/i2c-0"
  * For most later (starting already Q3/2012) Raspberry Pi machines, its typically "/dev/i2c-1"
  */
#define MPU6050_I2C_DEV_0 "/dev/i2c-0"
#define MPU6050_I2C_DEV_1 "/dev/i2c-1"
#define MPU6050_I2C_DEV_2 "/dev/i2c-2"
#define MPU6050_I2C_DEV_3 "/dev/i2c-3"
#define MPU6050_I2C_DEV_DEFAULT MPU6050_I2C_DEV_1

/** Possible I2C device addresses of the MPU6050
  * 0x68 is the default address
  */
#define MPU6050_ADDR_1 0x68
#define MPU6050_ADDR_2 0x69
 
/** Digital low pass filter settings
  * See the MPU6050 register map for details
  * http://invensense.com/mems/gyro/documents/RM-MPU-6000A-00v4.2.pdf
  */ 
enum EMPU6050LowPassFilterBandwidth
{
    MPU6050_DLPF_256HZ = 0x00,    //Gyro: 256Hz,  Accel: 260Hz
    MPU6050_DLPF_188HZ = 0x01,    //Gyro: 188Hz,  Accel: 184Hz
    MPU6050_DLPF_98HZ  = 0x02,    //Gyro:  98Hz,  Accel:  94Hz
    MPU6050_DLPF_42HZ  = 0x03,    //Gyro:  42Hz,  Accel:  44Hz
    MPU6050_DLPF_20HZ  = 0x04,    //Gyro:  20Hz,  Accel:  21Hz
    MPU6050_DLPF_10HZ  = 0x05,    //Gyro:  10Hz,  Accel:  10Hz
    MPU6050_DLPF_5HZ   = 0x06     //Gyro:   5Hz,  Accel:   5Hz
};

 
/** Part 1: Functions to access the MPU6050
 *
 */
  
/**
 * Container for quantities which can be expressed as 3 dimensional vector.
 * The axis system is to be assumed cartesian and right-handed
 * Measurement units depend on type of data:
 *   For accelerometer measurements, the unit is g
 *   For gyro measurements, the unit is degrees per seconds (deg/s)
 */
typedef struct 
{
  float x;        /**< Vector component of the quantity along the x-axis */
  float y;        /**< Vector component of the quantity along the y-axis */
  float z;        /**< Vector component of the quantity along the Z-axis */
} TMPU6050Vector3D;
  
/**
 * Initialize the MPU6050 access.
 * Must be called before using any of the other functions.
 * @param i2c_device the name of the i2c device on which the MPU6050 is attached
 * @param i2c_addr the I2C address of the MPU6050
 * @param bandwidth bandwidth of the digital low pass filter
 * @return true on success.
 */
bool mpu6050_init(const char* i2c_device = MPU6050_I2C_DEV_DEFAULT, uint8_t i2c_addr=MPU6050_ADDR_1, EMPU6050LowPassFilterBandwidth bandwidth=MPU6050_DLPF_256HZ); 

/**
 * Release the MPU6050 access.
 * @return true on success.
 */
bool mpu6050_deinit(); 

/**
 * Read the current acceleration, angular rate and temperature from the MPU6050
 * Any pointer may be NULL to indicate that the corresponding data is not requested
 * @param acceleration returns the acceleration vector. Measurement unit is g [1g = 9.80665 m/(s*s)]
 * @param gyro_angular_rate returns the angular rate vector as measured by gyroscope . Measurement unit is deg/s [degrees per seconds]
 * @param temperature returns the temperature in degrees celsius
 * @param timestamp returns a system timestamp in ms (milliseconds) derived from clock_gettime(CLOCK_MONOTONIC);
 * @return true on success.
 * @note: Never call this function while the callback mechanism is running!
 */
bool mpu6050_read_accel_gyro(TMPU6050Vector3D* acceleration, TMPU6050Vector3D* gyro_angular_rate, float* temperature, uint64_t* timestamp);

/** Part 2: Provide data via callback mechanism
 *
 */

/**
 * Callback type for MPU6050 data.
 * Use this type of callback if you want to register for MPU6050 data.
 * This callback may return buffered data (num_elements >1) depending on configuration
 * If the arrays contain buffered data (num_elements >1), the elements will be ordered with rising timestamps
 * Data in different arrays with the same index belong together
 * @param acceleration pointer to an array of TMPU6050Vector3D with size num_elements containing acceleration data
 * @param gyro_angular_rate pointer to an array of TMPU6050Vector3D with size num_elements containing angular rate data
 * @param temperature pointer to an array of float with size num_elements containing temperature data
 * @param timestamp pointer to an array of uint64_t with size num_elements containing timestamps
 * @param num_elements: allowed range: >=1. If num_elements >1, buffered data are provided. 
 */
typedef void (*MPU6050Callback)(const TMPU6050Vector3D acceleration[], const TMPU6050Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements);

/**
 * Register MPU6050 callback.
 * This is the recommended method for continuously accessing the MPU6050 data
 * The callback will be invoked when new MPU6050 data is available and the reader thread is running.
 * To start the reader thred, call mpu6050_start_reader_thread().
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool mpu6050_register_callback(MPU6050Callback callback);

/**
 * Deregister MPU6050 callback.
 * After calling this method no new MPU6050 data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool mpu6050_deregister_callback(MPU6050Callback callback);

/**
 * Start the MPU6050 reader thread.
 * This thread will call the callback function registered by mpu6050_register_callback()
 * The thread may be started before of after registering the callback function
 * @param sample_interval Interval in ms (milliseconds) at which MPU6050 data shall be read
 * @param num_samples Number of samples to read for one call of the callback function
 * @param average If true, the only the average (mean value) of the num_samples will be returned
 * @return True on success.
 * @note Be sure to select a meaningful combination of sample_interval and igital low pass filter bandwidth
 */
bool mpu6050_start_reader_thread(uint64_t sample_interval, uint16_t num_samples, bool average);

/**
 * Stop the MPU6050 reader thread.
 * @return True on success.
 */
bool mpu6050_stop_reader_thread();

/** Part 3: Utility functions and conversion factors
 *
 */

 /** Unit conversion factors
  *
  * MPU6050_UNIT_1_G: Standard gravity: 1g = 9.80665 m/(s*s)
  * @see http://en.wikipedia.org/wiki/Standard_gravity
  * @see http://www.bipm.org/utils/common/pdf/si_brochure_8_en.pdf#page=51
  * @see http://en.wikipedia.org/wiki/Gravitational_acceleration 
  *
  * MPU6050_UNIT_1_RAD_IN_DEG: Angle conversion from radian to degrees
  */
#ifndef M_PI
//No more available in C-99
#define MPU6050_PI (4.0*atan(1.0))
//#define M_PI 3.141592653589793238462643
#else
#define MPU6050_PI M_PI
#endif  
#define MPU6050_UNIT_1_G 9.80665
#define MPU6050_UNIT_1_RAD_IN_DEG (180.0/MPU6050_PI)

/**
 * Get system timestamp
 * @return returns a system timestamp in ms (milliseconds) derived from clock_gettime(CLOCK_MONOTONIC);
 */
uint64_t mpu6050_get_timestamp(); 


#ifdef __cplusplus
}
#endif

#endif //INCLUDE_MPU6050
