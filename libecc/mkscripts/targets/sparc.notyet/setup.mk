TARGET := sparc
Arch.$(TARGET) := sparc
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
