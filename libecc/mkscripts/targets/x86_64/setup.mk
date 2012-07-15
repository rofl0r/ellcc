TARGET := x86_64
Arch.$(TARGET) := x86_64
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
