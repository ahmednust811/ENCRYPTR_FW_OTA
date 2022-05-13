#
# Component Makefile
#

idf_component_register(SRCS "src/aws_iot_jobs_interface.c"
						"src/aws_iot_jobs_json.c"
				        "src/aws_iot_jobs_topics.c"
				        "src/aws_iot_jobs_types.c"
				        "src/aws_iot_json_utils.c"
				        "src/aws_iot_mqtt_client.c"
				        "src/aws_iot_mqtt_client_common_internal.c"
				        "src/aws_iot_mqtt_client_connect.c"
				        "src/aws_iot_mqtt_client_publish.c"
				        "src/aws_iot_mqtt_client_subscribe.c"
				        "src/aws_iot_mqtt_client_unsubscribe.c"
				        "src/aws_iot_mqtt_client_yield.c"
				        "src/aws_iot_shadow.c"
				        "src/aws_iot_shadow_actions.c"
				        "src/aws_iot_shadow_json.c"
				        "src/aws_iot_shadow_records.c"
				        "platform_wrap/timer.c"
				        "mbedtls_wrap/network_mbedtls_wrapper.c"
				        INCLUDE_DIRS "include" "platform_wrap" "mbedtls_wrap" "../../main"
		                REQUIRES jsmn mbedtls)
                  
# The integration code gives plenty of these warmings so they are dummed down in this component
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable -Wno-unused-variable)