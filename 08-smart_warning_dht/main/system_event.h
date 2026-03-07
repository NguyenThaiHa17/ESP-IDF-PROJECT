#ifndef SYSTEM_EVENT_H
#define SYSTEM_EVENT_H

typedef enum {
    EVENT_BUTTON_PRESS,
    EVENT_BUTTON_LONG,
    EVENT_TEMP
    // Thêm các event khác ở đây
} event_type_t;  // <--- QUAN TRỌNG: Phải có dấu chấm phẩy ở đây

typedef struct {
    event_type_t type;
    int value;
    float temperature;
} system_event_t; // <--- QUAN TRỌNG: Phải có dấu chấm phẩy ở đây

#endif