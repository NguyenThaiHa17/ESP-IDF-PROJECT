idf_component_register(SRCS "main.c" 
                            "button.c" 
                            "dht_task.c" 
                            "control.c"  # <--- PHẢI CÓ DÒNG NÀY
                       INCLUDE_DIRS ".")