#include <stdio.h>

#define NB -4.02

int main(int argc, char **argv) {
    int nb;
    printf("You've to found to mystery nb (1 - 10).\n");
    do {
        scanf("%d", &nb);
        if (nb > NB)
            printf("Lower.\n");
        else if (nb < NB)
            printf("Larger.\n");
    } while (nb != NB);
    printf("You've found the mystery number.\n");
    return 0;
}
