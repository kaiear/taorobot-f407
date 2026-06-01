#ifndef __USBH_HID_GAMEPAD_H
#define __USBH_HID_GAMEPAD_H

#include "usbh_hid_core.h"
#include "main.h"
#include "usbh_usr.h"

extern uint8_t ps2_buf[12];
extern uint8_t ps2_do_ok;
extern HID_cb_TypeDef HID_GAMEPAD_cb;

#endif
