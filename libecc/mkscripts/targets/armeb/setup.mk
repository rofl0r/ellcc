TARGET := armeb
Arch.$(TARGET) := armeb
TARGET.$(TARGET) := -target $(TARGET)-ellcc-$(OS)-eabi
CFLAGS.$(TARGET) := $(TARGET.$(TARGET)) -mcpu=armv6z -mfpu=vfp -mfloat-abi=softfp $(CFLAGS)
CXXFLAGS.$(TARGET) := $(TARGET.$(TARGET)) -mcpu=armv6z -mfpu=vfp -mfloat-abi=softfp $(CXXFLAGS)
LDFLAGS := $(TARGET.$(TARGET))
