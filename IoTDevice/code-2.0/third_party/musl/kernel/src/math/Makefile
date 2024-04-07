include $(LITEOSTOPDIR)/config.mk

all : $(LOCAL_COBJS_MATH)

$(LOCAL_COBJS_MATH): $(OBJOUT)/musl/src/math/%.o: %.c
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) -c $< -o $@

.PHONY: all