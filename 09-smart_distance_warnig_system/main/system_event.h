#ifndef SYSTEM_EVENT_H
#define SYSTEM_EVENT_H

typedef enum {
    EVENT_DISTANCE
} event_type_t;

typedef struct  
{
    event_type_t type ;
    float distance_cm ;
} system_event_t;
#endif
