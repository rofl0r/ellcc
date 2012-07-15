TARGET := i386
Arch.$(TARGET) := i386
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
