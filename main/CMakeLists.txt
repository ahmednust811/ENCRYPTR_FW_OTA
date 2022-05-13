idf_component_register(SRCS "app_main.c"
				    "ota_task.c"			
                    INCLUDE_DIRS "../components"
                   	)
#target_add_binary_data(${COMPONENT_TARGET} "certs/ota_ca_starfield.crt.pem" TEXT)
#target_add_binary_data(${COMPONENT_TARGET} "certs/ota_ca_baltimore.crt.pem" TEXT)
