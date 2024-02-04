#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>

#define INITIAL_SCALE 2.0
#define TARGET_SCALE 0.0001
#define NUM_IMAGES 50

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int numProcesses = atoi(argv[1]);

    double scaleStep = pow(TARGET_SCALE / INITIAL_SCALE, 1.0 / NUM_IMAGES);

    for (int i = 0; i < NUM_IMAGES; i++) {
        double currentScale = INITIAL_SCALE * pow(scaleStep, i);

        pid_t pid = fork();

        if (pid == 0) {
            char mandelCommand[256];
            snprintf(mandelCommand, sizeof(mandelCommand), "./mandel -x -0.5 -y 0 -s %lf -m 1000 -W 800 -H 800 -o mandel%d.bmp", currentScale, i);
            system(mandelCommand);
            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_IMAGES; i++) {
        int status;
        waitpid(-1, &status, 0);
    }

    printf("mandelmovie is creating %d images.\n", NUM_IMAGES);
    return 0;
}

