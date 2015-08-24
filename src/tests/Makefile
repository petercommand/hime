include ../../config.mak
.SUFFIXES:	.c .cpp .a .so .E .h
CFLAGS= -DUNIX=1 $(WALL) $(OPTFLAGS) $(GTKINC) -I../IMdkit/include -I../im-client \
        -DHIME_TABLE_DIR=\"$(HIME_TABLE_DIR)\" \
        -DHIME_OGG_DIR=\"$(HIME_OGG_DIR)\" \
        -DDOC_DIR=\"$(DOC_DIR)\" \
        -DHIME_DEFAULT_ICON_DIR=\"$(HIME_DEFAULT_ICON_DIR)\" \
        -DHIME_SCRIPT_DIR=\"$(HIME_SCRIPT_DIR)\" -DHIME_BIN_DIR=\"$(HIME_BIN_DIR)\" \
        -DSYS_ICON_DIR=\"$(SYS_ICON_DIR)\" -DFREEBSD=$(FREEBSD) -DMAC_OS=$(MAC_OS) \
        -DHIME_VERSION=\"$(shell head -n 1 ../../ChangeLog)\" \
        -DG_DISABLE_SINGLE_INCLUDES -DG_DISABLE_DEPRECATED \
        -DGDK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED \
        -DGTK_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE


SRCS = main.c test-common.c test-hime-event.c


OBJS = $(SRCS:%.c=%.o) ../hime-event.o ../hime-signal.o
BIN = hime-test

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(BIN)
	./hime-test

hime-test: $(OBJS) ../util.o
	@echo "linking $@ ..."
	@$(CCLD) -o $@ $^ $(LDFLAGS)
clean:
	rm -f $(BIN) $(SRCS:%.c=%.o)

.depend:
	@echo "building $@ ..."

	@$(CC) $(CPPFLAGS) $(CFLAGS) -MM *.c > $@

include .depend
