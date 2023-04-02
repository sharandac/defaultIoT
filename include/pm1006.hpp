/**
 * @file pm1006.hpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class vindriktning {
public:
	/**
	 * @brief Cinit softSerial on RX pin
	 */
	void begin( uint8_t _RXpin ) {
		_sensorRX.begin( 9600, SWSERIAL_8N1, _RXpin, -1 );
		_sensorRX.enableRx( true );
		_sensorRX.enableTx( false );
	}
	/**
	 * @brief end softSerial
	 */
	void end( void ) {
		_sensorRX.end();
	}
	/**
	 * @brief loop function and collect data from sensor
	 */
	void loop() {
		/**
		 * check if data is available
		 */
		if( !_sensorRX.available() )
			return;
		/**
		 * read data from sensor
		 */
		while( _sensorRX.available() ) {
			_rxBuf[_rxIdx++] = _sensorRX.read();
			if( 64 <= _rxIdx )
				clearRxBuf();
		}
		/**
		 * check if we have a complete message ?
		 */
		if ( _rxIdx >= 20 ) {
			/**
			 * check if the message has a valid header and checksum
			 */
			if( isValidHeader() && isValidChecksum() )
				collectData();
			else
				clearRxBuf();
		}
	}
	void debug( bool _enabled ) {
		_debug = _enabled;
	}
	/**
	 * @brief Check if 1.0µm particle value is valid
	 * 
	 * @return true or false
	 */
	bool isPm1Valid( void ) {
		return _sensor.pm[ 0 ].valid;
	}
	/**
	 * @brief Check if 2.5µm particle value is valid
	 * 
	 * @return true or false
	 */
	bool isPm25Valid( void ) {
		return _sensor.pm[ 1 ].valid;
	}
	/**
	 * @brief Check if 10µm particle value is valid
	 * 
	 * @return true or false
	 */
	bool isPm10Valid( void ) {
		return _sensor.pm[ 2 ].valid;
	}
	/**
	 * @brief Get the 1.0µm particle value
	 */
	uint16_t getPm1Value( void ) {
		return _sensor.pm[ 0 ].pm;
	}
	/**
	 * @brief Get the 2.5µm particle value
	 */
	uint16_t getPm25Value( void ) {
		return _sensor.pm[ 1 ].pm;
	}
	/**
	 * @brief Get the 10µm particle value
	 */
	uint16_t getPm10Value( void ) {
		return _sensor.pm[ 2 ].pm;
	}
	/**
	 * @brief Get the data frame as string
	 */
	const char* getDataFrame( void ) {
		return _dataFrame.c_str();
	}

private:
	/**
	 * @brief Struct for storing the state of the particle sensor
	 */
	typedef struct {
		uint16_t pm = 0;									/** @brief particle value */
		uint16_t measurements[ 5 ] = { 0, 0, 0, 0, 0 };		/** @brief measurement buffer */
		int measurementIdx = 0;								/** @brief measurement buffer index */
		bool valid = false;									/** @brief valid flag */
	} particleData_t;
	/**
	 * @brief Struct for storing the state of the particle sensor per particle size 
     */
	typedef struct {
		particleData_t pm[ 3 ];								/** @brief particle data */
	} particleSensorData_t;									/** @brief particle sensor data */
	/**
	 * @brief particle sensor data structure
	 */
	particleSensorData_t _sensor;			
	/**
	 * @brief The serial port to receive data from the sensor
	 */
	SoftwareSerial _sensorRX;								/** @brief soft serial interface */
	uint8_t _rxBuf[ 255 ];									/** @brief serial buffer */
	uint8_t _rxIdx = 0;										/** @brief serial puffer data index */
	uint8_t _lastRxBuf[ 255 ];								/** @brief last serial buffer */
	uint8_t _lastRxIdx = 0;									/** @brief last serial puffer data index */
	String _dataFrame = "";									/** @brief data frame string */
	bool _debug = false;									/** @brief debug flag */
	/**
	 * @brief Clear the serial buffer
	 */
	void clearRxBuf( void ) {
		memset( _rxBuf, 0, sizeof ( _rxBuf ) );
		_rxIdx = 0;
	}
	/**
 	 * @brief Check if the received message has a valid header
	 * 
	 * @return true or false
     */
	bool isValidHeader( void ) {
		/**
		 * check if the message has a valid header
		 */
		if( 0x16 == _rxBuf[ 0 ] && 0x11 == _rxBuf[ 1 ] && 0x0b == _rxBuf[ 2 ]  )
			return true;

		log_e("Received message with invalid header.");
		return false;
	}
	/**
	 * @brief Parse the received message and update the state
	 */
	void collectData( void ) {
		uint16_t _pm[ 3 ];
		/**
		 * get the particle values from the received message
		 */
		_pm[ 0 ] = (_rxBuf[ 9 ] << 8 ^ 1 ) | _rxBuf[ 10 ];
		_pm[ 1 ] = (_rxBuf[ 5 ] << 8 ) | _rxBuf[ 6 ];
		_pm[ 2 ] = (_rxBuf[ 13 ] << 8 ^ 1 ) | _rxBuf[ 14 ];
		/**
		 * build a string with the received data
		 */
		_dataFrame = "";
		for( size_t i = 0; i < 20; i++ ) {
			if( i )
				_dataFrame += ":";
			if( _rxBuf[ i ] < 16 )
				_dataFrame += "0";
			_dataFrame += String( _rxBuf[ i ], HEX );
		}
		/**
		 * put out debug information if enabled
		 */
		if( _debug ) {
			log_i("Received data: %s", _dataFrame.c_str() );
			log_i("PM1: %dµg/m³, PM2.5: %dµg/m³, PM10: %dµg/m³", _pm[ 0 ], _pm[ 1 ], _pm[ 2 ] );
		}
		/**
		 * get all particle values and store them for averaging
		*/
		for( size_t i = 0; i < 3; i++ ) {
			/**
			 * store the received data for avaeraging
			 */
			_sensor.pm[ i ].measurements[ _sensor.pm[ i ].measurementIdx ] = _pm[ i ];
			_sensor.pm[ i ].measurementIdx = (_sensor.pm[ i ].measurementIdx + 1 ) % 5;
			/**
			 * calculate the average of the last 5 measurements
			 */
			if ( _sensor.pm[ i ].measurementIdx == 0 ) {
				float _avgPm = 0.0f;

				for( uint8_t idx = 0; idx < 5; idx++ )
					_avgPm += _sensor.pm[ i ].measurements[ idx ] / 5.0f;

				_sensor.pm[ i ].pm = _avgPm;
				_sensor.pm[ i ].valid = true;
			}
		}
		/**
		 * clear the serial buffer
		 */
		memcpy( _lastRxBuf, _rxBuf, sizeof ( _rxBuf ) );
		clearRxBuf();
	}
	/**
	 * @brief Check if the received message has a valid checksum
	 * 
	 * @return true or false
	 */
	bool isValidChecksum() {
		uint8_t checksum = 0;

		for( size_t i = 0; i < 20; i++ )
			checksum += _rxBuf[ i ];

		if( checksum )
			log_e("Received message with invalid checksum. Expected: 0. Actual: %0x02x", checksum );

		return checksum == 0;
	}
};