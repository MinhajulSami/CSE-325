#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_tram = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_board = PTHREAD_COND_INITIALIZER;

int num_germans_waiting = 0;
int num_italians_waiting = 0;
int num_boarded = 0;

void print_line(const char *message) {
    printf("%s\n", message);
    fflush(stdout);
    usleep(500000); // Sleep for 500ms (0.5 seconds) between lines
}

void *german_thread(void *arg) {
    int id = *(int *)arg;
    char message[50];
    sprintf(message, "German player %d has arrived.", id);
    print_line(message);

    pthread_mutex_lock(&mutex);
    num_germans_waiting++;

    while (num_boarded == 4 || (num_germans_waiting > 0 && num_italians_waiting >= 3)) {
        pthread_cond_wait(&cond_board, &mutex);
    }

    num_boarded++;
    if (num_boarded == 4) {
        sprintf(message, "Tram departing with Germans: %d, Italians: %d.", num_germans_waiting, num_italians_waiting);
        print_line(message);
        num_germans_waiting = 0;
        num_italians_waiting = 0;
        num_boarded = 0;
        pthread_cond_broadcast(&cond_tram);
    }

    pthread_mutex_unlock(&mutex);

    sprintf(message, "German player %d has crossed the fly-over.", id);
    print_line(message);

    return NULL;
}

void *italian_thread(void *arg) {
    int id = *(int *)arg;
    char message[50];
    sprintf(message, "Italian player %d has arrived.", id);
    print_line(message);

    pthread_mutex_lock(&mutex);
    num_italians_waiting++;

    while (num_boarded == 4 || (num_italians_waiting > 0 && num_germans_waiting >= 3)) {
        pthread_cond_wait(&cond_board, &mutex);
    }

    num_boarded++;
    if (num_boarded == 4) {
        sprintf(message, "Tram departing with Germans: %d, Italians: %d.", num_germans_waiting, num_italians_waiting);
        print_line(message);
        num_germans_waiting = 0;
        num_italians_waiting = 0;
        num_boarded = 0;
        pthread_cond_broadcast(&cond_tram);
    }

    pthread_mutex_unlock(&mutex);

    sprintf(message, "Italian player %d has crossed the fly-over.", id);
    print_line(message);

    return NULL;
}

int main() {
    int num_germans, num_italians;

    printf("Enter the number of German players: ");
    scanf("%d", &num_germans);

    printf("Enter the number of Italian players: ");
    scanf("%d", &num_italians);

    if (num_germans < 1 || num_italians < 1) {
        printf("Invalid input. There must be at least 1 German and 1 Italian player.\n");
        return 1;
    }

    pthread_t german_threads[num_germans];
    pthread_t italian_threads[num_italians];
    int german_ids[num_germans];
    int italian_ids[num_italians];

    printf("FIFA World Cup Tram Simulation\n");

    for (int i = 0; i < num_germans; i++) {
        german_ids[i] = i + 1;
        pthread_create(&german_threads[i], NULL, german_thread, &german_ids[i]);
    }

    for (int i = 0; i < num_italians; i++) {
        italian_ids[i] = i + 1;
        pthread_create(&italian_threads[i], NULL, italian_thread, &italian_ids[i]);
    }

    for (int i = 0; i < num_germans; i++) {
        pthread_join(german_threads[i], NULL);
    }

    for (int i = 0; i < num_italians; i++) {
        pthread_join(italian_threads[i], NULL);
    }

    // Tram departure message after all players have crossed
    printf("Tram departing with Germans: %d, Italians: %d.\n", num_germans_waiting, num_italians_waiting);
    printf("All players have crossed the fly-over. Exiting simulation.\n");

    return 0;
}
