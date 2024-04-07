include $(LITEOSTOPDIR)/config.mk

all : $(LOCAL_COBJS_STDIO)

$(LOCAL_COBJS_STDIO): $(OBJOUT)/musl/src/stdio/%.o: %.c
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) -c $< -o $@

.PHONY: all