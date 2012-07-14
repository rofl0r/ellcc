TARGET := x86_64
Arch.$(TARGET) := x86_64
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
