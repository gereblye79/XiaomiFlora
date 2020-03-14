#include "XiaomiFlora.h"

/**************************************************************************/
/*!
    @brief class constructor
    @param mac Bluetooth MAC address of xiaomi flora device
*/
/**************************************************************************/
XiaomiFlora::XiaomiFlora(std::string mac) : _deviceAddress(mac) {
    //
}

/**************************************************************************/
/*!
    @brief inits bluetooth and connect to xiaomi flora
    @param deviceName Name of bluetooth device
*/
/**************************************************************************/
uint8_t XiaomiFlora::connect(void) {
    // check if connected
    if (_connected) {
        return 0;
    }

    // connect to device
    BLEDevice::init("");
    _client = BLEDevice::createClient();
    if (!_client->connect(_deviceAddress)) {
        return 1;
    }

    // check service uuid
    _remoteService = _client->getService(BLEUUID(SERVICE_UUID));
    if (_remoteService == nullptr) {
        _client->disconnect();
        return 2;
    }

    _connected = true;
    return 0;
}


/**************************************************************************/
/*!
    @brief disconnects from bluetooth device
*/
/**************************************************************************/
void XiaomiFlora::disconnect(void) {
    // check if connected
    if (!_connected) {
        return;
    }
    _client->disconnect();
    _connected = false;
    _client = NULL;
    _remoteService = NULL;
}

/**************************************************************************/
/*!
    @brief checks if connected to device and connection still open
    @return true if connected to device
*/
/**************************************************************************/
bool XiaomiFlora::isConnected(void) {
    // check if connected
    if (_connected) {
        // check if connection still open
        if(_client->isConnected()) {
            return true;
        }
        disconnect();
        return false;
    }
    return false;
}

/**************************************************************************/
/*!
    @brief Get signal strength to bluetooth device
    @return Signal strength percentage [0-100]
*/
/**************************************************************************/
uint8_t XiaomiFlora::getSignalStrength(void) {
    // check if connected
    if (!isConnected()) {
        return 0;
    }
    int rssi = _client->getRssi();

    if (rssi >= -50) {
        return 100;
    } else if (rssi <= -100) {
        return 0;
    }
    return 2 * (rssi + 100);
}

/**************************************************************************/
/*!
    @brief Sets the device to service mode to read data from it
    @return true if entered to service mode
*/
/**************************************************************************/
bool XiaomiFlora::forceServiceMode(void) {
	if (!isConnected()) {
		return false;
	}
	
	BLERemoteCharacteristic* floraChar = _remoteService->getCharacteristic(BLEUUID(WRITEMODE_UUID));
	if (floraChar == nullptr) {
		return false;
	}
	
	uint8_t buf[2] = {0xA0, 0x1F};
	floraChar->writeValue(buf, 2, true);
	
	delay(500);
	return true;
}

/**************************************************************************/
/*!
    @brief Get battery charge
    @return Battery charge percentage [0-100]
*/
/**************************************************************************/
String XiaomiFlora::getBattery(void) {
	if (!isConnected()) {
		return "NOT CONNECTED";
	}
	
	BLERemoteCharacteristic* floraChar = _remoteService->getCharacteristic(BLEUUID(BAT_UUID));
	if (floraChar == nullptr) {
		return "ERROR 1";
	}
	floraChar->toString().c_str();
	
	std::string value = floraChar->readValue();
	if (value.length() == 0) {
		return "ERROR 2";
	}
	
	return value.c_str();
}

/**************************************************************************/
/*!
    @brief Get flora data from device
    @return FloraData struct
*/
/**************************************************************************/
FloraData XiaomiFlora::getFloraData(void) {
	FloraData ret;
	if (!isConnected()) {
		return ret;
	}
	
	BLERemoteCharacteristic* floraChar = _remoteService->getCharacteristic(BLEUUID(SENSOR_UUID));
	if (floraChar == nullptr) {
		return ret;
	}
	floraChar->toString().c_str();
	
	std::string value = floraChar->readValue();
	ret = {((value[0] + value[1] * 256) / ((float)10.0)), value[7], (value[3]+ value[4] * 256), (value[8]+ value[9] * 256)};
	return ret;
}
