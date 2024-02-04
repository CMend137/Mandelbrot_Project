#include "bitmap.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

// gray color iteration
int iteration_to_color(int i, int max) {
    int gray = 255 * i / max;
    return MAKE_RGBA(gray, gray, gray, 0);
}

int iterations_at_point(double x, double y, int max);
void compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max);
void *thread_compute_image(void *thread_args);

struct ThreadArgs {
    struct bitmap *bm;
    double xmin, xmax, ymin, ymax;
    int max;
    int start_row;
    int end_row;
};

void show_help() {
    printf("Use: mandel [options]\n");
    printf("Where options are:\n");
    printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
    printf("-x <coord>  X coordinate of the image center point. (default=0)\n");
    printf("-y <coord>  Y coordinate of the image center point. (default=0)\n");
    printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
    printf("-W <pixels> Width of the image in pixels. (default=500)\n");
    printf("-H <pixels> Height of the image in pixels. (default=500)\n");
    printf("-o <file>   Set the output file. (default=mandel.bmp)\n");
    printf("-n <threads> Number of threads to use. (default=1)\n");
    printf("-h          Show this help text.\n");
    printf("\nSome examples are:\n");
    printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
    printf("mandel -x -0.38 -y -0.665 -s 0.05 -m 100\n");
    printf("mandel -x 0.286932 -y 0.014287 -s 0.0005 -m 1000 -n 4\n\n");
}

int main(int argc, char *argv[]) {
    char c;

    const char *outfile = "mandel.bmp";
    double xcenter = 0;
    double ycenter = 0;
    double scale = 4;
    int image_width = 500;
    int image_height = 500;
    int max = 1000;
    int num_threads = 1;

    while ((c = getopt(argc, argv, "x:y:s:W:H:m:o:n:h")) != -1) {
        switch (c) {
            case 'x':
                xcenter = atof(optarg);
                break;
            case 'y':
                ycenter = atof(optarg);
                break;
            case 's':
                scale = atof(optarg);
                break;
            case 'W':
                image_width = atoi(optarg);
                break;
            case 'H':
                image_height = atoi(optarg);
                break;
            case 'm':
                max = atoi(optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'n':
                num_threads = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(1);
                break;
        }
    }

    printf("mandel: x=%lf y=%lf scale=%lf max=%d threads=%d outfile=%s\n", xcenter, ycenter, scale, max, num_threads, outfile);

    struct bitmap *bm = bitmap_create(image_width, image_height);
    bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));

    pthread_t threads[num_threads];
    struct ThreadArgs thread_args[num_threads];
    int rows_per_thread = image_height / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_args[i].bm = bm;
        thread_args[i].xmin = xcenter - scale;
        thread_args[i].xmax = xcenter + scale;
        thread_args[i].ymin = ycenter - scale;
        thread_args[i].ymax = ycenter + scale;
        thread_args[i].max = max;
        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == num_threads - 1) ? image_height : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, thread_compute_image, (void *)&thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (!bitmap_save(bm, outfile)) {
        fprintf(stderr, "mandel: couldn't write to %s: %s\n", outfile, strerror(errno));
        return 1;
    }

    return 0;
}

void *thread_compute_image(void *thread_args) {
    struct ThreadArgs *args = (struct ThreadArgs *)thread_args;
    struct bitmap *bm = args->bm;
    double xmin = args->xmin;
    double xmax = args->xmax;
    double ymin = args->ymin;
    double ymax = args->ymax;
    int max = args->max;

    int width = bitmap_width(bm);
    int image_height = bitmap_height(bm);
    for (int j = args->start_row; j < args->end_row; j++) {
        for (int i = 0; i < width; i++) {
            double x = xmin + i * (xmax - xmin) / width;
            double y = ymin + j * (ymax - ymin) / image_height;
            int iters = iterations_at_point(x, y, max);
            bitmap_set(bm, i, j, iters);
        }
    }

    pthread_exit(NULL);
}

int iterations_at_point(double x, double y, int max) {
    double x0 = x;
    double y0 = y;
    int iter = 0;
    while ((x * x + y * y <= 4) && iter < max) {
        double xt = x * x - y * y + x0;
        double yt = 2 * x * y + y0;
        x = xt;
        y = yt;
        iter++;
    }
    return iteration_to_color(iter, max);
}
