TARGET := armeb
Arch.$(TARGET) := armeb
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS)-eabi -mcpu=armv4t $(CFLAGS)
