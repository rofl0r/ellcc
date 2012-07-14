TARGET := ppc64
Arch.$(TARGET) := ppc64
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
