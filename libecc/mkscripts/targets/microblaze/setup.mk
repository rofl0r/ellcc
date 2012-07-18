TARGET := microblaze
Arch.$(TARGET) := microblaze
TARGET.$(TARGET) := -target $(TARGET)-ellcc-$(OS)
CFLAGS.$(TARGET) := $(TARGET.$(TARGET)) $(CFLAGS)
CXXFLAGS.$(TARGET) := $(TARGET.$(TARGET)) $(CXXFLAGS)
LDFLAGS := $(TARGET.$(TARGET))
