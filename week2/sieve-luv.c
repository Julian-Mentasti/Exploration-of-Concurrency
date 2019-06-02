#include <stdlib.h>
#include <stdio.h>
#include <utils.h>
#include <uv.h>

// You hold all my primes
static int *prime_array;
// You tell me how many primes to hold
static int calc_ammount;
// You let me hold key and value
struct Container
{
    int position;
    int value;
};
// You work
void worker (uv_async_t* handle)
{
    // Get the data from the channel
    struct Container *data = handle->data;
    // get the position of the next prime
    int position = data->position;
    int value = data->value;

    // If we have found the number of primes needed
    if (position == calc_ammount) {
        // Close the Channel
        uv_close((uv_handle_t*)handle, NULL);
        return;
    }

    if (position == -1) {
        // Generator sequence
        data->position = 0;
        // Increment counter
        data->value = value+1;
        // wake up the event loop and call the handler's callback
        // Essentially initiating a new routine.
        uv_async_send(handle);
        return;
    }
    // Was number to filter
    int myNum = prime_array[position];

    if (!myNum) {
        // Save the new prime
        prime_array[position] = value;
        // Go back to the first prime
        data->position = 0;
        // Signal
        uv_async_send(handle);
        return;
    }

    if ( value % myNum) {
        // Try Again
        data->position = position+1;
        uv_async_send(handle);
    } else {
        // Goto generator sequence
        data->position = -1;
        uv_async_send(handle);
    }
}

int main (int argc, char* argv[])
{

    // Get the command line argument
    // hope its not a string
    calc_ammount = atoi(argv[1]);

    // Here is were I would do checking
    // But I like to live dangerously

    // Allocate the array of primes
    prime_array = calloc(calc_ammount, sizeof(int));

    // Set 2 to be the first prime found
    prime_array[0] = 2;    // Event Loop

    // Initiate default loop because we fruity
    uv_loop_t *loop = uv_default_loop();

    // Used for callbacks and also workrs as a channel
    uv_async_t handle;

    // initiate the loop with the handler and a function
    uv_async_init(loop, &handle, worker);

    // Allocate memory for the channel
    struct Container *hotainter = malloc(sizeof(struct Container));
    hotainter->position = -1;
    hotainter->value = 1;
    handle.data = hotainter;

    // wake up the event loop and call the handler's callback
    uv_async_send(&handle);


    // Runs the event loop until there are no more active and referenced handles or requests.
    uv_run(loop, UV_RUN_DEFAULT);

    // Print

    for (int i = 0; i < calc_ammount; i++) {
        printf(" %d\n", prime_array[i]);
    }

    // Let the memory be free
    free(prime_array);
}


