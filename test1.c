#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "myvar.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int main(int argc, char **argv) {
    int miproc, numproc;    // Rango del proceso actual, número de procesos
    MPI_Status status;
    unsigned char *img = NULL;
    int width, height, channels;
    const char *name;
    size_t img_size;
    unsigned char *red_img = NULL;

    MPI_Init(&argc, &argv); /* Inicializar MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &miproc); /* Determinar el rango del proceso invocado*/
    MPI_Comm_size(MPI_COMM_WORLD, &numproc); /* Determinar el numero de procesos */

    if (miproc == 0) { // Proceso maestro
        if (argc < 2) {
            printf("Selecciona tu imagen");
            MPI_Finalize();
            return 0;
        }

        name = argv[1];
        img = stbi_load(name, &width, &height, &channels, 0);

        if (img == NULL) {
            printf("Error al cargar la imagen\n");
            MPI_Finalize();
            return 1;
        }

        printf("Imagen cargada con un ancho de %dpx, un alto de %dpx y %d canales\n", width, height, channels);

        img_size = width * height * channels;
        red_img = (unsigned char*)malloc(img_size);
        if (red_img == NULL) {
            printf("No se pudo asignar memoria para la imagen en escala de rojos.\n");
            MPI_Finalize();
            return 1;
        }

        for (unsigned char *p = img, *pr = red_img; p != img + img_size; p += channels, pr += channels) {
            *pr = *p;        // Valor rojo del píxel
            *(pr + 1) = 0;   // Canal verde
            *(pr + 2) = 0;   // Canal azul
            if (channels == 4) {
                *(pr + 3) = *(p + 3);   // Canal de transparencia (si existe)
            }
        }
    }

    MPI_Bcast(&img_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    if (miproc != 0) {
        red_img = (unsigned char*)malloc(img_size);
        if (red_img == NULL) {
            printf("No se pudo asignar memoria para la imagen en escala de rojos.\n");
            MPI_Finalize();
            return 1;
        }
    }

    MPI_Bcast(red_img, img_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (miproc == 0) { // Proceso maestro
        stbi_write_png("flor_rojos.png", width, height, channels, red_img, width * channels);
        stbi_image_free(img);
        free(red_img);
    } else {
        free(red_img);
    }

    MPI_Finalize();
    return 0;
}
