TARGET := microblaze
Arch.$(TARGET) := microblaze
CFLAGS.$(TARGET) := -target $(Arch.$(TARGET))-ellcc-$(OS) $(CFLAGS)
