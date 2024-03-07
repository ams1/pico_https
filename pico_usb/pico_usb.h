#ifndef _PICO_USB_H
#define _PICO_USB_H

#ifdef __cplusplus
extern "C" {
#endif

void send_keyboard_report(uint8_t keyCode, uint32_t duration);
void pico_usb_update();
    
#ifdef __cplusplus
}
#endif

#endif
