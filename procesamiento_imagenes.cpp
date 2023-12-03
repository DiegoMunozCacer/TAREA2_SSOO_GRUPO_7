#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <omp.h>
#include <thread>

#ifdef PROCESO_PARALELO_HILOS
#include <vector>
#endif

using namespace cv;
using namespace std;

#include <opencv2/core.hpp>

// Función para convertir RGB a escala de grises usando el método de luminosidad

void convertirAGrises(Mat& entrada, Mat& salida, int filaInicio, int filaFin) {
    for (int r = filaInicio; r < filaFin; r++) {
        for (int c = 0; c < entrada.cols; c++) {
            Vec3b pixel = entrada.at<Vec3b>(r, c);
            int valorGris = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
            salida.at<uchar>(r, c) = valorGris;
        }
    }
}

// Función para procesar la imagen secuencialmente
void procesamientoSecuencial(Mat& entrada, Mat& salida) {
    convertirAGrises(entrada, salida, 0, entrada.rows);
}

// Función para procesar la imagen en paralelo usando hilos
void procesamientoParaleloHilos(Mat& entrada, Mat& salida, int numHilos) {
    vector<thread> hilos;
    int filasPorHilo = entrada.rows / numHilos;

    for (int i = 0; i < numHilos; i++) {
        int filaInicio = i * filasPorHilo;
        int filaFin = (i == numHilos - 1) ? entrada.rows : (i + 1) * filasPorHilo;

        hilos.emplace_back(convertirAGrises, ref(entrada), ref(salida), filaInicio, filaFin);
    }

    for (auto& hilo : hilos) {
        hilo.join();
    }
}

// Función para procesar la imagen en paralelo usando OpenMP
void procesamientoParaleloOpenMP(Mat& entrada, Mat& salida, int numHilos) {
    
    for (int r = 0; r < entrada.rows; r++) {
        convertirAGrises(entrada, salida, r, r + 1);
    }
}

int main(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
        cerr << "Uso: " << argv[0] << " <imagen_entrada> <imagen_salida> [num_hilos]" << endl;
        return EXIT_FAILURE;
    }

    // Lee la imagen
    Mat imagenEntrada = imread(argv[1], IMREAD_COLOR);
    if (imagenEntrada.empty()) {
        cerr << "Error: No se pudo leer la imagen." << endl;
        return EXIT_FAILURE;
    }

    // Crea una matriz de salida para la imagen en escala de grises
    Mat imagenSalida(imagenEntrada.rows, imagenEntrada.cols, CV_8U);
    int numHilos = 1; // Cambia esto a (int numHilos=1;) Por defecto a 1 hilo para la versión secuencial

#ifdef _OPENMP
    if (argc == 4) {
        // Si OpenMP está disponible y el usuario proporciona un tercer argumento, utilícelo como el número de hilos
        numHilos = stoi(argv[3]);
    } else {
        // Si OpenMP está disponible y el usuario no proporciona un tercer argumento, le pregunta al usuario el número de hilos
        cout << "Ingrese el número de hilos: ";
        cin >> numHilos;
    }
#endif

    // Medir el tiempo de ejecución usando crono
    auto tiempoInicio = chrono::high_resolution_clock::now();

    // Elija entre procesamiento secuencial, paralelo basado en hilos o paralelo OpenMP
    // Comenta esto si no quieres el procesamiento secuencial
    procesamientoSecuencial(imagenEntrada, imagenSalida);

#ifdef PROCESO_PARALELO_HILOS
    if (argc == 4) {
        numHilos = stoi(argv[3]);
    } else {
        // Si el procesamiento paralelo basado en hilos está disponible y el usuario no proporciona un tercer argumento, le pregunta al usuario el número de hilos
        cout << "Ingrese el número de hilos: ";
        cin >> numHilos;
    }
    procesamientoParaleloHilos(imagenEntrada, imagenSalida, numHilos);
#else
    procesamientoParaleloOpenMP(imagenEntrada, imagenSalida, numHilos);
#endif

    auto tiempoFin = chrono::high_resolution_clock::now();
    auto duracion = chrono::duration_cast<chrono::microseconds>(tiempoFin - tiempoInicio).count() / 1e6;

    // Guarda la imagen en escala de grises
    imwrite(argv[2], imagenSalida);

  // Salida del tiempo de ejecución
cout << "Loading image . . ." << endl;
cout << "Rows (height): " << imagenEntrada.rows << " Cols (width): " << imagenEntrada.cols << endl;
cout << "Start conversion . . ." << endl;
cout << "End conversion . . ." << endl;
cout << "Total time spent in seconds is " << duracion << "." << endl;


    return EXIT_SUCCESS;
}
