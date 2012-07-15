TARGET := mipsel
Arch.$(TARGET) := mipsel
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) -mcpu=mips32r2 $(CFLAGS)
