TARGET := armeb
Arch.$(TARGET) := armeb
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS)-eabi -mcpu=armv6z -mfpu=vfp -mfloat-abi=softfp $(CFLAGS)
