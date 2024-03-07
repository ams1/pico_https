add_library(pico_usb INTERFACE)
target_sources(pico_usb INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.c
  ${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/pico_usb.cpp
  )

target_link_libraries(pico_usb INTERFACE pico_logger pico_unique_id tinyusb_device tinyusb_board)
target_include_directories(pico_usb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
