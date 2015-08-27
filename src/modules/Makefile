include ../../config.mak
include ../suffixes-rule

CFLAGS+=-I.. -I../IMdkit/include
HIME_MODULE=intcode-module.so

ifeq ($(USE_ANTHY),Y)
HIME_MODULE+=anthy-module.so
CFLAGS += $(ANTHY_INC)
endif

ifeq ($(USE_CHEWING),Y)
HIME_MODULE+=chewing-module.so
CHEWING_DATADIR=$(shell pkg-config --variable=datadir chewing)
CFLAGS += -DCHEWING_DATADIR=\"$(CHEWING_DATADIR)\" $(shell pkg-config --cflags chewing)
endif

ifeq ($(USE_TSIN),Y)
HIME_MODULE+=tsin-module.so
endif

all:	$(HIME_MODULE)

anthy_module_so = anthy/anthy.pico
anthy-module.so: $(anthy_module_so)
	@echo "linking $@ ..."
	@$(CCLD) $(SO_FLAGS) -o $@ $(anthy_module_so) $(LDFLAGS) $(ANTHY_INC)

intcode_module_so = intcode/intcode.pico intcode/win-int.pico
intcode-module.so: $(intcode_module_so)
	@echo "linking $@ ..."
	@$(CCLD) $(SO_FLAGS) -o $@ $(intcode_module_so) $(LDFLAGS)

chewing_module_obj = chewing/chewing-conf.pico
chewing_module_so = chewing/chewing.pico chewing/hime-setup-chewing.pico
chewing-module.so: $(chewing_module_obj) $(chewing_module_so)
	@echo "linking $@ ..."
	@$(CCLD) $(SO_FLAGS) -o $@ $(chewing_module_obj) $(chewing_module_so) $(LDFLAGS) $(shell pkg-config --cflags --libs chewing)

tsin-module_obj = tsin/phrase-save-menu.pico
tsin-module_so = tsin/tsin.pico
tsin-module.so: $(tsin_module_so)
	@echo "linking $@ ..."
	@$(CCLD) $(SO_FLAGS) -o $@ $(tsin_module_obj) $(tsin_module_so) $(LDFLAGS)


install:
	install $(HIME_MODULE) $(himelibdir)

clean:
	@echo "clean up"
	@rm -f chewing/*.pico chewing/*.so chewing*.o anthy/*.pico anthy/*.so anthoy/*.o intcode/*.pico intcode/*.so intcode/*.o tsin/*.o tsin/*.pico tsin/*.so
