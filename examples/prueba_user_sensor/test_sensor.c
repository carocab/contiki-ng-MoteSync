/**
 * @brief prueba interrupcion sensor
 * @author  MoteSync - Proyecto RSI - 2018
 * @date 5/11/2018
 * @version 1.0
 *
 **/

#include "contiki.h"
#include <stdio.h>
#include "dev/user-sensor.h"
#include "sys/rtimer.h"
/*----------------------------------------------------------------------------*/
struct data_sensor_t data_user_sensor;

PROCESS(test_sensor, "Test user sensor");

AUTOSTART_PROCESSES(&test_sensor);

/*----------------------------------------------------------------------------*/


PROCESS_THREAD(test_sensor, ev, data)
{
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(user_sensor);
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &user_sensor);
		data_user_sensor = get_data_sensor();
		printf("\n");
		printf("evento en el tiempo = ");
		printf("%lu \n",data_user_sensor.event_time);
    	printf("\n");
	}
	PROCESS_END();
}


