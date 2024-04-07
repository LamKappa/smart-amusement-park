include $(LITEOSTOPDIR)/config.mk

all : $(LOCAL_COBJS_STRING)

$(LOCAL_COBJS_STRING): $(OBJOUT)/musl/src/string/%.o: %.c
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) -c $< -o $@

.PHONY: all