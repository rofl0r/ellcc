TARGET := armeb
Arch.$(TARGET) := armeb
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS)-eabi -mcpu=armv6z -mfpu=vfp -mfloat-abi=softfp $(CFLAGS)
