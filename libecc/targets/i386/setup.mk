TARGET := i386
Arch.$(TARGET) := i386
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
