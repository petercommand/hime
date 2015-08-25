#ifndef HIME_CHPHO_H
#define HIME_CHPHO_H
typedef struct CHPHO {
  char *ch;
  char cha[CH_SZ+1];
  phokey_t pho;
  u_short flag;
  char psta; // phrase start index
} CHPHO;

enum {
  FLAG_CHPHO_FIXED=1,    // user selected the char, so it should not be changed
  FLAG_CHPHO_PHRASE_HEAD=2,
  FLAG_CHPHO_PHRASE_USER_HEAD=4,
  FLAG_CHPHO_PHRASE_VOID=8,
  FLAG_CHPHO_PHRASE_BODY=16,
  FLAG_CHPHO_PHO_PHRASE=32,
  FLAG_CHPHO_PINYIN_TONE=64,
  FLAG_CHPHO_GTAB_BUF_EN_NO_SPC=128,
  FLAG_CHPHO_PHRASE_TAIL=0x100,
};

#endif //HIME_CHPHO_H
