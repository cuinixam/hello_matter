# Create empty list of device sources
set(device_srcs "devices/status_led.cpp")
set(device_includes "devices")
message(STATUS "CONFIG_MATTER_COMPONENT_LIGHT: ${CONFIG_MATTER_COMPONENT_LIGHT}")
message(STATUS "CONFIG_MATTER_COMPONENT_TEMP_SENSOR: ${CONFIG_MATTER_COMPONENT_TEMP_SENSOR}")
if(CONFIG_MATTER_COMPONENT_LIGHT STREQUAL "y")
    # append "devices/light/device_driver.cpp" to the list
    list(APPEND device_srcs "devices/light/device_driver.cpp")
    list(APPEND device_srcs "devices/light/analog_led_lamp.cpp")
    list(APPEND device_srcs "devices/led_param.cpp")
    list(APPEND device_includes "devices/light")
elseif(CONFIG_MATTER_COMPONENT_TEMP_SENSOR STREQUAL "y")
    # append "devices/light/device_driver.cpp" to the list
    list(APPEND device_srcs "devices/temp_sensor/device_driver.cpp")
    list(APPEND device_srcs "devices/temp_sensor/shtc3.cpp")
    list(APPEND device_srcs "devices/led_param.cpp")
    list(APPEND device_includes "devices/temp_sensor")
endif()

idf_component_register(
    SRCS  "app_main.cpp" "utils/qr_code_utils.cpp" ${device_srcs}
    PRIV_INCLUDE_DIRS  "." "utils" "${ESP_MATTER_PATH}/examples/common/utils" ${device_includes}
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
