#ifndef __XiaomiFlora_H__
#define __XiaomiFlora_H__

#include <Arduino.h>
#include <BLEDevice.h>
#include <time.h>

#define   SERVICE_UUID "00001204-0000-1000-8000-00805f9b34fb"
#define       BAT_UUID "00001a02-0000-1000-8000-00805f9b34fb"
#define    SENSOR_UUID "00001a01-0000-1000-8000-00805f9b34fb"
#define WRITEMODE_UUID "00001a00-0000-1000-8000-00805f9b34fb"

struct FloraData {
	float temperature;
	uint16_t moisture;
	uint16_t light;
	uint16_t conductivity;
};

class XiaomiFlora {
	public:
		// constructors
		XiaomiFlora(std::string mac);
		
		//public methods
		uint8_t connect(void);
        uint8_t getSignalStrength(void);
		void disconnect(void);
		bool isConnected(void);
		
		//getter
		String getBattery(void);
		FloraData getFloraData(void);
		
		//setter
		bool forceServiceMode(void);
	
	private:
		BLEAddress _deviceAddress;
		bool _connected = false;
		BLEClient* _client;
		BLERemoteService* _remoteService;
};

#endif