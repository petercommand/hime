/* Copyright (C) 2011 Edward Der-Hua Liu, Hsin-Chu, Taiwan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef HIME_MODULE_CB_H
#define HIME_MODULE_CB_H
#include "hime-event.h"
#include "hime-module.h"


#ifdef __cplusplus
extern "C" {
#endif
  int module_init_win(HIME_module_main_functions *funcs);
  void module_win_geom();
  int module_reset();
  int module_get_preedit(char *str, HIME_PREEDIT_ATTR attr[], int *pcursor, int *compose_flag);
  gboolean module_feedkey(KeySym kv, u_int kvstate);
  int module_feedkey_release(KeySym xkey, int kbstate);
  void module_move_win(int x, int y);
  void module_change_font_size();
  void module_show_win();
  void module_hide_win();
  int module_win_visible();
  int module_flush_input();
#ifdef __cplusplus
}
#endif

///////// for hime main() only
typedef struct _HIME_module_callback_functions {
  int (*module_init_win)(HIME_module_main_functions *funcs);
  //Initialize the input method module & its corresponding window, do not show the input method window after this function is call (hide the window), wait for module_show_win() to show the input method window
  void (*module_get_win_geom)();
  void (*module_set_win1_cb)();
  int (*module_reset)();
  int (*module_get_preedit)(char *str, HIME_PREEDIT_ATTR attr[], int *pcursor, int *compose_flag);
  gboolean (*module_feedkey)(KeySym keysym, int kvstate);
  int (*module_feedkey_release)(KeySym xkey, int kbstate);
  void (*module_move_win)(int x, int y);
  void (*module_change_font_size)();
  void (*module_show_win)();
  void (*module_hide_win)();
  int (*module_win_visible)();
  int (*module_flush_input)();
  int (*module_event_handler)(HIME_EVENT eve);
  //The event handler is used for handling events that has a default action.
  // return true if the event is processed (ie: a custom handler is implemented for the event
  // return false if you want hime to call the default handler
  void (*module_setup_window_create)();
} HIME_module_callback_functions;

#endif //HIME_MODULE_CB_H