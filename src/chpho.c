#include "chpho.h"
#include "hime.h"
#include "pho.h"


void chpho_extract(CHPHO *chph, int len, phokey_t *pho, char *ch)
{
  int i;
  int ofs=0;
  ch[0]=0;

  for(i=0; i < len; i++) {
    if (pho)
      pho[i] = chph[i].pho;

    char *str = chph[i].ch;
    strcat(ch + ofs, str);
    ofs+=strlen(str);
  }
//   dbg("chpho_extract %s\n", ch);
}

void init_chpho_i(CHPHO *chpho, int i)
{
//  dbg("init_chpho_i %d\n", i);
  chpho[i].ch = chpho[i].cha;
  chpho[i].ch[0]=' ';
  chpho[i].ch[1]=0;
  chpho[i].flag=0;
  chpho[i].psta=-1;
}

void clear_match(int *phrase_start)
{
  *phrase_start = -1;
}

// in tsin db, # of phokey = # of character, use this to extract only the first characer
static void chpho_extract_cha(CHPHO *chph, int len, phokey_t *pho, char *ch)
{
  int i;
  int ofs=0;

  for(i=0; i < len; i++) {
    if (pho)
      pho[i] = chph[i].pho;
    ofs += u8cpy(ch + ofs, chph[i].ch);
  }

  ch[ofs]=0;
//   dbg("chpho_extract %s\n", ch);
}

void chpho_get_str(CHPHO *chpho, int len, char *ch, int idx)
{
  int ofs=0, i;
  for(i=0; i < len; i++) {
    int u8len = u8cpy(&ch[ofs], chpho[idx+i].ch);
    ofs+=u8len;
  }

  ch[ofs]=0;
}

void clr_ch_buf(CHPHO *chpho, int *phrase_start)
{
  int i;
  for(i=0; i < MAX_PH_BF_EXT; i++) {
    init_chpho_i(chpho, i);
  }

  clear_match(phrase_start);
}