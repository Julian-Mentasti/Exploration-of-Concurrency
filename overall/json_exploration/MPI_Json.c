#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "json-c/json.h"
#include <string.h>

int rank;
int size;
int BUFFER_SIZE = 1024;

// jsonHandler
void jsonHandler(void *buf)
{
	FILE *fp;
	struct json_object *parsed_json;
	struct json_object *name;
	struct json_object *age;
	struct json_object *friends;
	struct json_object *friend;
	size_t n_friends;
	size_t i;

	fp = fopen("test.json","r");

	fread(buf, 1024, 1, fp);
	fclose(fp);
	return;
}

// This thread will send data to the speaker
void reader(int rank, int size)
{
    printf("reader at %d\n", rank);
    char* outbox = calloc(BUFFER_SIZE, sizeof(char));
    jsonHandler(outbox);

    MPI_Send(outbox, BUFFER_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    free(outbox);
    return;
}

// This thread will print the data from the reader.
void speaker(int rank, int size)
{
    printf("speaker at %d\n", rank);
    char* inbox = calloc(BUFFER_SIZE, sizeof(char));
    MPI_Recv(inbox, BUFFER_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	struct json_object *parsed_json;
	struct json_object *name;
	struct json_object *age;
	struct json_object *friends;
	parsed_json = json_tokener_parse(inbox);

	json_object_object_get_ex(parsed_json, "name", &name);
	json_object_object_get_ex(parsed_json, "age", &age);
	json_object_object_get_ex(parsed_json, "friends", &friends);

	printf("Name: %s\n", json_object_get_string(name));
	printf("Age: %d\n", json_object_get_int(age));
	printf("Friends: %d\n", json_object_get_int(friends));

    free(inbox);
    return;
}

int main(int argc, char** argv)
{

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank) {
        speaker(rank, size);
    } else {
        reader(rank, size);
    }

    MPI_Finalize();
    return 0;
}
