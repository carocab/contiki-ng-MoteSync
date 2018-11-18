/**
*   Proyecto RSI - MoteSync
*   Año: 2018
*   Grupo: Bentancour, Cabrera, Díaz
*
*   path: $(CONTIKI)/arch/platform/zoul/dev/user-sensor.c
*   brief: Driver para el sensor de sonido.
*   Basado en $(CONTIKI)/arch/platform/zoul/dev/motion-sensor.c
*   
*/


#include "contiki.h"
#include "dev/i2c.h"
#include "dev/user-sensor.h"
#include "lib/sensors.h"
#include "dev/sys-ctrl.h"
#include "dev/gpio.h"
#include "dev/gpio-hal.h"
#include "dev/ioc.h"
#include "net/mac/tsch/tsch-asn.h" //Para el tipo struct tsch_asn_t
#include "net/mac/tsch/tsch-sync.h" //Para las funciones get_asn y get_slot_start

/*---------------------------------------------------------------------------*/

/**
 * Timer para filtrar
 */

#define USER_SENSOR_FILTER_DURATION (CLOCK_SECOND)

static struct timer filtertimer;


/*---------------------------------------------------------------------------*/
static uint32_t event_time;
static struct tsch_asn_t event_asn;

/*---------------------------------------------------------------------------*/
/** 
 * Pin para conexión del sensor.
 * 
 */
#define USER_SENSOR_PORT_BASE  GPIO_PORT_TO_BASE(USER_SENSOR_PORT)
#define USER_SENSOR_PIN_MASK   GPIO_PIN_MASK(USER_SENSOR_PIN)

/*---------------------------------------------------------------------------*/

static struct data_sensor_t data_sensor;

/*---------------------------------------------------------------------------*/
/*
 * Devuelve datos del ultimo evento
 */
struct data_sensor_t get_data_sensor(void){
       return data_sensor;
}

/*---------------------------------------------------------------------------*/
/*
 * Handler para la interrupcion del sensor. Obtiene inicio y ASN del slot actual
 * y tiempo de ocurrencia de la interrupcion.
 */
static void
user_interrupt_handler(gpio_hal_pin_mask_t pin_mask)
{
  event_time = RTIMER_NOW();
  if(!timer_expired(&filtertimer))
  {
    return;
  }
  else
  {
	timer_set(&filtertimer, USER_SENSOR_FILTER_DURATION);
    data_sensor.ref_time = get_slot_start();
    event_asn = get_asn();
    data_sensor.asn_ls4b = event_asn.ls4b;
    data_sensor.asn_ms1b = event_asn.ms1b;
    data_sensor.event_time = event_time;
  	sensors_changed(&user_sensor);
  }
}
/*---------------------------------------------------------------------------*/
/*
 * Struct para agregar a la lista de handlers de los GPIO para el módulo 
 * gpio_hal (os/dev/gpio-hal.{h,c})
 */
static gpio_hal_event_handler_t user_handler = {
  .next = NULL,
  .handler = user_interrupt_handler,
  .pin_mask = gpio_hal_pin_to_mask(USER_SENSOR_PIN) << (USER_SENSOR_PORT << 3),
};

/*---------------------------------------------------------------------------*/

  /*
   * Configuracion del GPIO para la interrupción por flancos de bajada
   */

static int
configure(int type, int value)
{
  if(type != USER_ACTIVE) {
    return USER_ERROR;
  }

  if(!value) /* Disable sensor*/
  { 
    GPIO_DISABLE_INTERRUPT(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    return USER_SUCCESS;
  }
  else /* Enable sensor*/
  {
    GPIO_SOFTWARE_CONTROL(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    GPIO_SET_INPUT(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    GPIO_DETECT_FALLING(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    GPIO_TRIGGER_SINGLE_EDGE(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    ioc_set_over(USER_SENSOR_PORT, USER_SENSOR_PIN, IOC_OVERRIDE_DIS);
    //ioc_set_over(USER_SENSOR_PORT, USER_SENSOR_PIN, IOC_OVERRIDE_PUE); //PUE = Pull up Enable (Buscar en cc2538)
    gpio_hal_register_handler(&user_handler);
    GPIO_ENABLE_INTERRUPT(USER_SENSOR_PORT_BASE, USER_SENSOR_PIN_MASK);
    NVIC_EnableIRQ(USER_SENSOR_VECTOR);
  }
  return USER_SUCCESS;
}

/*---------------------------------------------------------------------------*/

/*
 * Inicilizacion del sensor en el modulo del sistema 'sensors' (os/lib/sensors.{c,h})
 */

SENSORS_SENSOR(user_sensor, USER_SENSOR, NULL /*No value function*/ , configure, NULL/*No status function*/);

/*---------------------------------------------------------------------------*/
