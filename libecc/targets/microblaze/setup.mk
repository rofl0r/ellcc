TARGET := microblaze
Arch.$(TARGET) := microblaze
CFLAGS.$(TARGET) := -target $(TARGET)-ellcc-$(OS) $(CFLAGS)
