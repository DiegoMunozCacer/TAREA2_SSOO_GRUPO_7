CXX = g++
CXXFLAGS = -std=c++11 -Wall `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

all: procesamiento_secuencial procesamiento_paralelo_hilos procesamiento_paralelo_openmp

procesamiento_secuencial: procesamiento_imagenes.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

procesamiento_paralelo_hilos: procesamiento_imagenes.cpp
	$(CXX) $(CXXFLAGS) -DPROCESO_PARALELO_HILOS -o $@ $< $(LDFLAGS) -lpthread

procesamiento_paralelo_openmp: procesamiento_imagenes.cpp
	$(CXX) $(CXXFLAGS) -DPROC_OPENMP -o $@ $< $(LDFLAGS) -fopenmp

clean:
	rm -f procesamiento_secuencial procesamiento_paralelo_hilos procesamiento_paralelo_openmp