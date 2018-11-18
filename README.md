# Readme

Para descargar el repo ejecutar:

```
git clone https://github.com/carocab/contiki-ng-MoteSync.git
cd contiki-ng-MoteSync
git submodule update --init --recursive
```

## Driver del sensor
**Implementacion:**

[user-sensor.c](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/dev/user-sensor.c)

[user-sensor.h](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/dev/user-sensor.h)

Basado en [motion-sensor.h](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/dev/motion-sensor.h)  [motion-sensor.c](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/dev/motion-sensor.c)
                
**Archivos modificados para incluir al sensor:**

[zoul-sensors.c](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/dev/zoul-sensors.c)

[Makefile.zoul](https://github.com/carocab/contiki-ng-MoteSync/blob/master/arch/platform/zoul/Makefile.zoul)

[Makefile.include](https://github.com/carocab/contiki-ng-MoteSync/blob/master/Makefile.include)

**Programa de prueba:**

https://github.com/carocab/contiki-ng-MoteSync/tree/master/examples/prueba_user_sensor

                
## TSCH
*Funciones para extraer* `current_slot_start` y `tsch_current_asn`: [tsch-sync.h](https://github.com/carocab/contiki-ng-MoteSync/blob/master/os/net/mac/tsch/tsch-sync.h).
*Implementadas en:* [tsch-slot-operation.c](https://github.com/carocab/contiki-ng-MoteSync/blob/master/os/net/mac/tsch/tsch-slot-operation.c).


## Programa principal de los nodos

https://github.com/carocab/contiki-ng-MoteSync/tree/master/examples/Node

Comunicación UDP basada en [rpl-udp](https://github.com/carocab/contiki-ng-MoteSync/tree/master/examples/rpl-udp) example.

