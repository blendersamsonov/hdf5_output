HDF5_INCLUDE_PATH = /usr/include/hdf5/serial/
HDF5_LIBRARY_PATH = /usr/lib/x86_64-linux-gnu/hdf5/serial/

CXX := g++
CXXFLAGS := -O3 -march=native -ffast-math -std=c++11 -Wall -Wextra

CXXLIBS := -lhdf5 -lhdf5_cpp
INCLUDEDIR = 
ifdef HDF5_INCLUDE_PATH
INCLUDEDIR = -I$(HDF5_INCLUDE_PATH)
endif
LIBDIR = 
ifdef HDF5_LIBRARY_PATH
LIBDIR = -L$(HDF5_LIBRARY_PATH) -Wl,-rpath=$(HDF5_LIBRARY_PATH)
endif

test: main.cpp
	$(CXX) -o test main.cpp $(CXXFLAGS) $(CXXLIBS) $(LIBDIR) $(INCLUDEDIR) 