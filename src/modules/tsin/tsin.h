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
#ifndef HIME_TSIN_H
#define HIME_TSIN_H

#include <glib.h>
#include "../../hime.h"
#include "../../pho.h"
#define TSIN_GTAB_KEY "!!!!gtab-keys"

extern int phcount;
extern int hashidx[];
typedef struct {
  struct CHPHO *chpho;
  int c_idx, c_len;
  int ph_sta;  // phrase start
  int sel_pho;
//  int save_frm, save_to;
  int current_page;
  int startf;
  gboolean full_match;
  gboolean tsin_buffer_editing;

  int last_cursor_idx;
  int pho_menu_idx;
} TSIN_ST;
extern TSIN_ST tss;






void extract_pho(int chpho_idx, int plen, phokey_t *pho);
gboolean tsin_seek(void *pho, int plen, int *r_sti, int *r_edi, char *tone_off);
void load_tsin_entry(int idx, char *len, usecount_t *usecount, void *pho, u_char *ch);
gboolean check_fixed_mismatch(int chpho_idx, char *mtch, int plen);
void show_button_pho(gboolean bshow);
char *tsin_get_chpho_pinyin_set(char *set_arr);
void show_tsin_stat();


typedef struct {
  char signature[32];
  int version, flag;
  int keybits, maxkey;
  char keymap[128];
} TSIN_GTAB_HEAD;


extern gboolean tsin_is_gtab;


#endif //HIME_TSIN_H
