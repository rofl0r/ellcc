TARGET := ppc
Arch.$(TARGET) := ppc
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
