#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{

    int fid = open("/dev/input/event6", O_RDONLY);
    if (fid == 0) {
        fprintf(stderr, "couldn't open file\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "File Opened\n");

    struct input_event event;
    int nbytes = read(fid, &event, sizeof(event));
    int value = event.value;
    while(read(fid, &event, sizeof(event))) {
        if (event.type != 0) {
            if (event.code == 1) {
                if (event.value < value) {
                    //pointer mouse naik
                    printf("naik\n");
                } else {
                    //pointer mouse turun
                    printf("turun\n");
                }
            } else {
                if (event.value < value)
                {
                    //pointer mouse kiri
                    printf("kiri\n");
                }
                else
                {
                    //pointer mouse kanan
                    printf("kanan\n");
                }
            }
            value = event.value;
        }
    }

    return 0;
}