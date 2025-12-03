PYTHON ?= python3

EXT_SUFFIX := $(shell $(PYTHON) -c 'import sysconfig;print(sysconfig.get_config_var("EXT_SUFFIX") or ".so")')
PYBIND11_INCLUDES := $(shell $(PYTHON) -m pybind11 --includes)
INSTALL_DIR := $(shell $(PYTHON) -c 'import sysconfig;print(sysconfig.get_path("platlib"))')
UNAME_S := $(shell uname -s 2>/dev/null || echo)

ifeq ($(origin CXX), undefined)
  ifneq ($(shell command -v clang++ >/dev/null 2>&1 && echo y),)
    CXX := clang++
  else ifneq ($(shell command -v g++ >/dev/null 2>&1 && echo y),)
    CXX := g++
  else ifneq ($(shell command -v cl >/dev/null 2>&1 && echo y),)
    CXX := cl
  else
    CXX := c++
  endif
endif

CXX_NAME := $(notdir $(CXX))
ifneq (,$(findstring cl,$(CXX_NAME)))
  CC_FAMILY := msvc
else ifneq (,$(findstring clang,$(CXX_NAME)))
  CC_FAMILY := clang
else ifneq (,$(findstring g++,$(CXX_NAME)))
  CC_FAMILY := gcc
else
  CC_FAMILY := generic
endif

SOURCES = hhc_python.cpp
TARGET = k_hhc$(EXT_SUFFIX)

.PHONY: all clean install

ifeq ($(CC_FAMILY),msvc)

  OBJEXT = .obj
  OBJECTS = $(SOURCES:.cpp=$(OBJEXT))
  CXXFLAGS ?= /std:c++17 /O2 /EHsc /MD
  LDFLAGS ?= /LD
  PY_LIBDIR := $(shell $(PYTHON) -c "import sysconfig;print(sysconfig.get_config_var('LIBDIR') or '')")
  PY_LIB := $(shell $(PYTHON) -c "import sysconfig;print(sysconfig.get_config_var('LIBRARY') or '')")
  PY_LIBS := $(shell $(PYTHON) -c "import sysconfig;print(sysconfig.get_config_var('LIBS') or '')")

  all: $(TARGET)

  $(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) /link /OUT:"$(TARGET)" /LIBPATH:"$(PY_LIBDIR)" $(PY_LIB) $(PY_LIBS) $(K_HHC_LIB)

  %.obj: %.cpp
	$(CXX) $(CXXFLAGS) $(PYBIND11_INCLUDES) $(K_HHC_INC) /c $< /Fo$@

else

  OBJEXT = .o
  OBJECTS = $(SOURCES:.cpp=$(OBJEXT))
  CXXFLAGS ?= -std=c++17 -O3 -fPIC
  ifeq ($(UNAME_S),Darwin)
    LDFLAGS ?= -bundle -undefined dynamic_lookup
  else
    LDFLAGS ?= -shared
  endif
  PY_LDFLAGS := $(shell (command -v $(PYTHON)-config >/dev/null 2>&1 && $(PYTHON)-config --ldflags) || $(PYTHON) -c 'import sysconfig;print(" ".join(filter(None,[sysconfig.get_config_var("LIBS") or "", sysconfig.get_config_var("SYSLIBS") or "", sysconfig.get_config_var("LINKFORSHARED") or ""])))')

  all: $(TARGET)

  $(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) $(PY_LDFLAGS) $(K_HHC_LIB)

  %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PYBIND11_INCLUDES) $(K_HHC_INC) -c $< -o $@

endif

clean:
	rm -f $(TARGET) $(OBJECTS)

install: $(TARGET)
	$(PYTHON) -c "import sysconfig,shutil; shutil.copy2('$(TARGET)', sysconfig.get_path('platlib'))"