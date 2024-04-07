include $(LITEOSTOPDIR)/config.mk

all : $(LOCAL_COBJS_TIME)

$(LOCAL_COBJS_TIME): $(OBJOUT)/musl/src/time/%.o: %.c
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) -c $< -o $@

.PHONY: all