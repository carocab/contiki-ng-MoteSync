/**
*   Proyecto RSI - MoteSync
*   Año: 2018
*   Grupo: Bentancour, Cabrera, Díaz
*   path: $(CONTIKI)/arch/platform/zoul/dev/user-sensor.h
*
*   Basado en $(CONTIKI)/arch/platform/zoul/dev/motion-sensor.h
*   
*/

#include "lib/sensors.h"

/* -------------------------------------------------------------------------- */
#ifndef USER_SENSOR_H_
#define USER_SENSOR_H_


/* -------------------------------------------------------------------------- */
/** 
 * Pin para conexión del sensor.
 * 
 */
#ifndef USER_SENSOR_PORT
#define USER_SENSOR_PORT       GPIO_C_NUM
#endif
#ifndef USER_SENSOR_PIN
#define USER_SENSOR_PIN        3
#endif
#ifndef USER_SENSOR_VECTOR
#define USER_SENSOR_VECTOR     GPIO_C_IRQn
#endif
/* -------------------------------------------------------------------------- */
/**
 * User-sensor return and operation values
 */
#define USER_ACTIVE     SENSORS_ACTIVE
#define USER_SUCCESS    0
#define USER_ERROR      (-1)

/* -------------------------------------------------------------------------- */

/**
 * Identifier for the sensor in the sensors os module.
 */
#define USER_SENSOR "user_sensor"

/* -------------------------------------------------------------------------- */

 struct data_sensor_t {
  uint32_t ref_time; 
  uint32_t asn_ls4b;
  uint8_t asn_ms1b;
  uint32_t event_time; 
};


/**
 * Identifier for the sensor in the sensors os module.
 */
extern const struct sensors_sensor user_sensor;
/* -------------------------------------------------------------------------- */

/**
  * Devuelve estructura con el tiempo de ocurrencia del ultimo evento y el ASN 
  * y el inicio de slot de referencia para el evento del sensor.
  */
struct data_sensor_t get_data_sensor(void);

/* -------------------------------------------------------------------------- */
#endif /* ifndef USER_SENSOR_H_ */
