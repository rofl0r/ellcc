TARGET := arm
Arch.$(TARGET) := arm
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS)-eabi -mcpu=armv6z -mfpu=vfp -mfloat-abi=softfp $(CFLAGS)
