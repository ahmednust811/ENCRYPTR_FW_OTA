idf_component_register(SRCS "timed_ota.c" "app_main.c"
				    "ota_task.c"
					"wifi_connect.c"
					"uart_task.c"	
					"mqtt.c"		
                    INCLUDE_DIRS "../components"
                   	)
target_add_binary_data(${COMPONENT_TARGET} "certs/ota_ca_starfield.crt.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/ota_ca_baltimore.crt.pem" TEXT)
