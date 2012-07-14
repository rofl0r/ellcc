TARGET := mipsel
Arch.$(TARGET) := mipsel
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) -mcpu=mips32r2 $(CFLAGS)
