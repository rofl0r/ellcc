TARGET := mips
Arch.$(TARGET) := mips
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) -mcpu=mips32r2 $(CFLAGS)
