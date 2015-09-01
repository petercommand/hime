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
#ifndef GST_H
#define GST_H


#include <X11/X.h>
#include <glib.h>
#include <sys/types.h>

typedef enum {
  SAME_PHO_QUERY_none = 0,
  SAME_PHO_QUERY_gtab_input = 1,
  SAME_PHO_QUERY_pho_select = 2,
} SAME_PHO_QUERY;



#define MAX_TAB_KEY_NUM64_6 (10)

typedef struct {
  int S1, E1, last_idx, wild_page, pg_idx, total_matchN, sel1st_i;
  u_int64_t kval;
  gboolean last_full, wild_mode, spc_pressed, invalid_spc, more_pg, gtab_buf_select;
  short defselN, exa_match, ci, gbufN, gbuf_cursor;
  KeySym inch[MAX_TAB_KEY_NUM64_6];
} GTAB_ST;
extern GTAB_ST gtab_st;


#endif