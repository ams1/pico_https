#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "tls_listener.h"
#include "request_handler.h"

#include "wifi.h"
#include "pico_logger.h"

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include <deque>

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void)
{
}

void tud_umount_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
}

void tud_resume_cb(void)
{
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    return 0;
}

void getHidKeyCode(char key, uint8_t *keyCode)
{
    switch (key)
    {
    case 'A': keyCode[0]=HID_KEY_A; break;
    case 'B': keyCode[0]=HID_KEY_B; break;
    case 'C': keyCode[0]=HID_KEY_C; break;
    case 'D': keyCode[0]=HID_KEY_D; break;
    case 'E': keyCode[0]=HID_KEY_E; break;
    case 'F': keyCode[0]=HID_KEY_F; break;
    case 'G': keyCode[0]=HID_KEY_G; break;
    case 'H': keyCode[0]=HID_KEY_H; break;
    case 'I': keyCode[0]=HID_KEY_I; break;
    case 'J': keyCode[0]=HID_KEY_J; break;
    case 'K': keyCode[0]=HID_KEY_K; break;
    case 'L': keyCode[0]=HID_KEY_L; break;
    case 'M': keyCode[0]=HID_KEY_M; break;
    case 'N': keyCode[0]=HID_KEY_N; break;
    case 'O': keyCode[0]=HID_KEY_O; break;
    case 'P': keyCode[0]=HID_KEY_P; break;
    case 'Q': keyCode[0]=HID_KEY_Q; break;
    case 'R': keyCode[0]=HID_KEY_R; break;
    case 'S': keyCode[0]=HID_KEY_S; break;
    case 'T': keyCode[0]=HID_KEY_T; break;
    case 'U': keyCode[0]=HID_KEY_U; break;
    case 'V': keyCode[0]=HID_KEY_V; break;
    case 'W': keyCode[0]=HID_KEY_W; break;
    case 'X': keyCode[0]=HID_KEY_X; break;
    case 'Y': keyCode[0]=HID_KEY_Y; break;
    case 'Z': keyCode[0]=HID_KEY_Z; break;
    case '1': keyCode[0]=HID_KEY_1; break;
    case '2': keyCode[0]=HID_KEY_2; break;
    case '3': keyCode[0]=HID_KEY_3; break;
    case '4': keyCode[0]=HID_KEY_4; break;
    case '5': keyCode[0]=HID_KEY_5; break;
    case '6': keyCode[0]=HID_KEY_6; break;
    case '7': keyCode[0]=HID_KEY_7; break;
    case '8': keyCode[0]=HID_KEY_8; break;
    case '9': keyCode[0]=HID_KEY_9; break;
    case '0': keyCode[0]=HID_KEY_0; break;
    default:
        keyCode[0]=0; break;
        // todo: encode rest of https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
    }
}

struct KeyToSend
{
    uint8_t keyCode;
    uint32_t end;
};

std::deque<KeyToSend> keysToSend;
bool sendingKey = false;
bool waitingForReportCb = false;
extern "C" void send_keyboard_report(uint8_t keyCode, uint32_t duration)
{
    trace("pico_usb::send_keyboard_report: %c %d", (char)keyCode, duration);

    KeyToSend key;
    key.keyCode = keyCode;
    key.end = keysToSend.empty() ? to_us_since_boot(get_absolute_time())/1000 + duration : keysToSend.back().end + duration;

    keysToSend.push_back(key);
}

extern "C" void pico_usb_update()
{
    if (waitingForReportCb)
    {
        return;
    }

    if (keysToSend.empty())
    {
        return;
    }
    
    if (sendingKey && static_cast<int32_t>(keysToSend.front().end - to_us_since_boot(get_absolute_time())/1000) < 0)
    {
        trace("pico_usb::tud_hid_keyboard_report - finished");
        
        // Key finished pressing
        keysToSend.pop_front();
        sendingKey = false;
        waitingForReportCb = true;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        return;
    }

    if (!sendingKey)
    {
        sendingKey = true;
        waitingForReportCb = true;

        uint8_t keyCodeArray[6] = { 0 };
        getHidKeyCode(keysToSend.front().keyCode, &keyCodeArray[0]);

        trace("pico_usb::tud_hid_keyboard_report - start keyCode[%d]", (uint32_t)keyCodeArray[0]);

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keyCodeArray);
        return;
    }    
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    waitingForReportCb = false;
}
