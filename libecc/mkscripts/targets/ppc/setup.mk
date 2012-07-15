TARGET := ppc
Arch.$(TARGET) := ppc
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
