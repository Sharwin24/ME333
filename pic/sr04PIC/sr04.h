#ifndef SR04__H__
#define SR04__H__

#define CORE_TICKS_PER_SECOND 24000000 // 24 Million ticks/sec
#define SEC_TO_TICK(s) ((s) * CORE_TICKS_PER_SECOND)
#define US_PER_TICK 24 // number of microseconds per core timer tick
#define SEC_TO_US(s) ((s) * 1000000)

void delay_us(unsigned int us);
void SR04_Startup();
unsigned int SR04_read_raw(unsigned int);
float SR04_read_meters();

#endif // SR04__H__