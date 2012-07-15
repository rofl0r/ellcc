TARGET := ppc64
Arch.$(TARGET) := ppc64
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
