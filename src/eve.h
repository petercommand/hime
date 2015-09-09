#ifndef HIME_EVE_H
#define HIME_EVE_H

#include "hime-module-cb.h"

HIME_module_callback_functions *module_cb();
void set_engine_process_key_event_cb();
gboolean hime_pho_mode();
KeySym keypad_proc(KeySym xkey);
#endif //HIME_EVE_H
