include $(LITEOSTOPDIR)/config.mk

all : $(LOCAL_COBJS_INTERNAL)

$(LOCAL_COBJS_INTERNAL): $(OBJOUT)/musl/src/internal/%.o: %.c
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) -c $< -o $@

.PHONY: all