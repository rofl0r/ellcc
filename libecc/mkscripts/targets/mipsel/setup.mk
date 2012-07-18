TARGET := mipsel
Arch.$(TARGET) := mipsel
TARGET.$(TARGET) := -target $(TARGET)-ellcc-$(OS)
CFLAGS.$(TARGET) := $(TARGET.$(TARGET)) -mcpu=mips32r2 $(CFLAGS)
CXXFLAGS.$(TARGET) := $(TARGET.$(TARGET)) -mcpu=mips32r2 $(CXXFLAGS)
LDFLAGS := $(TARGET.$(TARGET))
