// I2Cdev library collection - L3G4200D I2C device class header file
// Based on STMicroelectronics L3G4200D datasheet rev. 3, 12/2010
// TODO Add release date here
// [current release date] by Jonathan "j3rn" Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// TODO Add initial release date here as well
// Changelog:
//     [YYYY-mm-dd] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jonathan Arnett, Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "L3G4200D.h"

/** Default constructor, uses default I2C address.
 * @see L3G4200D_DEFAULT_ADDRESS
 */
L3G4200D::L3G4200D() {
    devAddr = L3G4200D_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see L3G4200D_DEFAULT_ADDRESS
 * @see L3G4200D_ADDRESS
 */
L3G4200D::L3G4200D(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void L3G4200D::initialize() {
    setPowerOn(true);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool L3G4200D::testConnection() {
    return getDeviceID() == 0b11010011;
}

// WHO_AM_I register, read-only

/** Get the Device ID.
 * The WHO_AM_I register holds the device's id
 * @return the device ID (should be 0b11010011, 109, 0x69)
 * @see L3G4200D_RA_WHO_AM_I
 */
uint8_t L3G4200D::getDeviceID() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer);
    return buffer[0];
}

// CTRL_REG1 register, r/w

/** Set the data output rate
 * @param rate The new data output rate (can be 100, 200, 400, or 800)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
void setDataOutputRate(uint16_t rate) {
	uint8_t writeVal;
	
	if (rate == 100) {
		writeVal = L3G4200D_RATE_100;
	} else if (rate == 200) {
		writeVal = L3G4200D_RATE_200;
	} else if (rate == 400) {
		writeVal = L3G4200D_RATE_400;
	} else {
		// Default/fallback is 800
		writeVal = L3G4200D_RATE_800;
	}
	
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ODR_BIT,
		L3G4200D_ODR_LENGTH, writeVal); 
}

/** Get the current data output rate
 * @return the current data output rate
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
uint16_t getDataOutputRate() {
	// Get rate from device
	uint8_t rate = I2Cdec::readBits(devAddr, L3G4200D_RA_CTRL_REG1,
		L3G4200D_ODR_BIT, L3G4200D_ODR_LENGTH, buffer);
	
	// Return the matching rate
	if (rate == L3G4200D_RATE_100) {
		return 100;
	} else if (rate == L3G4200D_RATE_200) {
		return 200;
	} else if (rate == L3G4200D_RATE_400) {
		return 400;
	}

	// We can assume that if it is not one of the previous, it is 800
	return 800;
}

/** Set the bandwidth cut-off mode
 * @param mode The new bandwidth cut-off mode
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
void setBandwidthCutOffMode(uint8_t mode) {
	// Assume cutOff is a valid mode
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, mode);
}

/** Get the current bandwidth cut-off mode
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
uint8_t getBandwidthCutOffMode() {
	return I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, buffer);
}

/** Gets the current bandwidth cutoff based on ODR and BW
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
float getBandwidthCutOff() {
	// Get necessary info from device
	int8_t dataRate = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, 
		L3G4200D_ODR_BIT, L3G4200D_ODR_LENGTH, buffer);
	int8_t bandwidthMode = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, 
		L3G4200D_BW_BIT, L3G4200D_BW_LENGTH, buffer);

	// Compute cut-off based on table in datasheet
	if (dataRate == L3G4200D_RATE_100) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else {
			return 25.0;
		}
	} else if (dataRate == L3G4200D_RATE_200) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 70.0;
		}
	} else if (dataRate == L3G4200D_RATE_400) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 20.0;
		} else if (bandwidthMode == L3G4200D_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 110.0;
		}
	} else {
		// L3G4200D_RATE_800 assumed
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 30.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 35.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 110.0;
		}
	}
}

/** Set power on or off (true and false, respectively)
 * @param enabled The new power setting
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
void setPowerOn(bool on) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, on);
}

/** Get the current power state
 * @return The powered on state (true == on)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
bool getPowerOn() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, buffer);
}

/** Enables or disables the ability to get Z data
 * @param enabled The new enabled state of the Z axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ZEN_BIT
 */
void setZEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ZEN_BIT, enabled);
}

/** Get whether Z axis data is enabled
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ZEN_BIT
 */
bool getZEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ZEN_BIT, 
		buffer);
}

/** Enables or disables the ability to get Y data
 * @param enabled The new enabled state of the Y axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_YEN_BIT
 */
void setYEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_YEN_BIT, enabled);
}

/** Get whether Y axis data is enabled
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_YEN_BIT
 */
bool getYEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_YEN_BIT, 
		buffer);
}

/** Enables or disables the ability to get X data
 * @param enabled The new enabled state of the X axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_XEN_BIT
 */
void setXEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_XEN_BIT, enabled);
}

/** Get whether X axis data is enabled
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_XEN_BIT
 */
bool getXEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_XEN_BIT, 
		buffer);
}










