#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <stdlib.h>
#include <mpi.h>
#include "simple.pb.h"

int rank;
int size;

int main()
{
    // MPI
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* This is the buffer where we will store our message. */
    // uint8_t buffer[128]; // = malloc(sizeof(uint8_t)*128);
    uint8_t* send_buffer = malloc(128*sizeof(uint8_t));
    uint8_t buffer[128];
    size_t message_length;
    bool status;


    if (rank) {
    /* Encode our message */
        {
            printf("Reader at %d\n", rank);
            /* Allocate space on the stack to store the message data.
             *
             * Nanopb generates simple struct definitions for all the messages.
             * - check out the contents of simple.pb.h!
             * It is a good idea to always initialize your structures
             * so that you do not have garbage data from RAM in there.
             */
            SimpleMessage message = SimpleMessage_init_zero;

            /* Create a stream that will write to our buffer. */
            pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

            /* Fill in the lucky number */
            message.lucky_number = 13;

            /* Now we are ready to encode the message! */
            status = pb_encode(&stream, SimpleMessage_fields, &message);
            message_length = stream.bytes_written;

            /* Then just check for any errors.. */
            if (!status)
            {
                printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
                return 1;
            }

            for (int i = 0; i < 128; ++i) {
                *(send_buffer + i*sizeof(uint8_t)) = buffer[i];
            }

            MPI_Send(&buffer, 10, MPI_UINT8_T, 1, 0, MPI_COMM_WORLD);
        }
    } else {
    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */

    /* But because we are lazy, we will just decode it immediately. */

        {
            printf("Speaker at %d\n", rank);


            uint8_t buffer2[128];
            MPI_Recv(&buffer, 128, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            /* Allocate space for the decoded message. */
            SimpleMessage message = SimpleMessage_init_zero;

            /* Create a stream that reads from the buffer. */
            pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

            /* Now we are ready to decode the message. */
            status = pb_decode(&stream, SimpleMessage_fields, &message);

            /* Check for errors... */
            if (!status)
            {
                printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
                return 1;
            }

            /* Print the data contained in the message. */
            printf("Your lucky number was %d!\n", (int)message.lucky_number);
        }
    }

    MPI_Finalize();
    return 0;
}

