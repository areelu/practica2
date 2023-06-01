#include <mpi.h>
#include <spng.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT_PROCESS 0

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char* input_filename = "input.png";
    const char* output_filename = "output.png";

    if (argc >= 2) {
        input_filename = argv[1];
    }

    spng_ctx* ctx;
    struct spng_ihdr ihdr;
    unsigned char* image;
    unsigned char* green_image;
    size_t image_size, green_image_size;

    if (rank == ROOT_PROCESS) {
        // Root process reads the input image
        FILE* input_file = fopen(input_filename, "rb");
        if (!input_file) {
            fprintf(stderr, "Error opening input file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        ctx = spng_ctx_new(0);
        if (!ctx) {
            fprintf(stderr, "Error creating spng_ctx\n");
            fclose(input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
        spng_set_png_file(ctx, input_file);

        int ret = spng_get_ihdr(ctx, &ihdr);
        if (ret) {
            fprintf(stderr, "Error getting image header: %s\n", spng_strerror(ret));
            spng_ctx_free(ctx);
            fclose(input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        image_size = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8);
        if (image_size == 0) {
            fprintf(stderr, "Error getting image size\n");
            spng_ctx_free(ctx);
            fclose(input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        image = (unsigned char*)malloc(image_size);
        if (!image) {
            fprintf(stderr, "Error allocating memory for image\n");
            spng_ctx_free(ctx);
            fclose(input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
        if (ret) {
            fprintf(stderr, "Error decoding image: %s\n", spng_strerror(ret));
            spng_ctx_free(ctx);
            fclose(input_file);
            free(image);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fclose(input_file);
    }

    // Broadcast image header to all processes
    MPI_Bcast(&ihdr, sizeof(struct spng_ihdr), MPI_BYTE, ROOT_PROCESS, MPI_COMM_WORLD);

    // Calculate the portion of the image to process for each process
    size_t image_portion_size = image_size / size;
    size_t image_portion_start = rank * image_portion_size;
    size_t image_portion_end = (rank == size - 1) ? image_size : image_portion_start + image_portion_size;

    // Allocate memory for the green image portion
    size_t green_image_portion_size = (image_portion_end - image_portion_start) / 4;
    green_image = (unsigned char*)malloc(green_image_portion_size);
    if (!green_image) {
        fprintf(stderr, "Error allocating memory for green image portion\n");
        spng_ctx_free(ctx);
        free(image);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Extract the green channel from the image portion
    for (size_t i = 0; i < green_image_portion_size; i++) {
        green_image[i] = image[(image_portion_start + i * 4) + 1];
    }

    // Gather the green image portions from all processes to the root process
    MPI_Gather(green_image, green_image_portion_size, MPI_UNSIGNED_CHAR,
               green_image, green_image_portion_size, MPI_UNSIGNED_CHAR,
               ROOT_PROCESS, MPI_COMM_WORLD);

    if (rank == ROOT_PROCESS) {
        // Root process creates the output image
        spng_ctx* output_ctx = spng_ctx_new(SPNG_CTX_IGNORE_ADLER32);
        if (!output_ctx) {
            fprintf(stderr, "Error creating spng_ctx for output image\n");
            spng_ctx_free(ctx);
            free(image);
            free(green_image);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        spng_set_ihdr(output_ctx, &ihdr);

        int ret = spng_set_png_file(output_ctx, fopen(output_filename, "wb"));
        if (ret) {
            fprintf(stderr, "Error setting PNG file for output image: %s\n", spng_strerror(ret));
            spng_ctx_free(ctx);
            free(image);
            free(green_image);
            spng_ctx_free(output_ctx);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        ret = spng_encode_image(output_ctx, green_image, green_image_size, SPNG_FMT_G8, 0);
        if (ret) {
            fprintf(stderr, "Error encoding output image: %s\n", spng_strerror(ret));
            spng_ctx_free(ctx);
            free(image);
            free(green_image);
            spng_ctx_free(output_ctx);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fclose(spng_get_png_file(output_ctx));
        spng_ctx_free(output_ctx);
    }

    free(image);
    free(green_image);
    spng_ctx_free(ctx);
    MPI_Finalize();

    return 0;
}
