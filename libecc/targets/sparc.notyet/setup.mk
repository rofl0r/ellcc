TARGET := sparc
Arch.$(TARGET) := sparc
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
