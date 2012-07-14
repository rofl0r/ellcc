TARGET := arm
Arch.$(TARGET) := arm
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS)-eabi -mcpu=armv4t $(CFLAGS)
