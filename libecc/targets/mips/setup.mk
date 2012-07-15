TARGET := mips
Arch.$(TARGET) := mips
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) -mcpu=mips32r2 $(CFLAGS)
