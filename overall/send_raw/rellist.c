#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get the new data field limit
int* dataFieldLimit(int* start_address, int item_size, int item_numbers)
{
    return start_address + (item_size * item_numbers);
}

// Add value in address and return new offset, if negative there was a boundary error
int insertFirstRelative(int* address, int value, int item_size, int offset, int* limit)
{
    int* new_address = address + offset + item_size;
    if (address + offset + item_size > limit)
        return -1;
    *(address + offset) = value;
    return offset + item_size;
}

// Return an updated offset removing the last value
int deleteFristRelative(int item_size, int offset)
{
    return offset - item_size;
}

// Print the list
void printRelative(int* address, int item_size, int offset)
{
    int* position = address;
    int* end = address + offset;
    while (position < end) {
        printf("[%d]", *position);
        position += item_size;
    }
    printf("\n");
}

// Return the length of the list
int lengthRelative(int item_size, int offset)
{
    return offset/item_size;
}

// appends the char into the end of the buffer
int append(char* buff, char c, int position, int limit)
{
    if (limit < position) return -1;
    buff[position] = c;
    position += 1;
    return position;
}

// Return the memory range as a char buffer
int liftData(char* buff, int* address, int item_size, int data_offset, int buff_offset)
{
    int* position = address;
    int* end = address + data_offset;
    int loop_counter = buff_offset;
    while (position < end) {
        char temp = *position + '0';
        loop_counter = append(buff,temp,loop_counter,data_offset);
        position += item_size;
    }
    return 1;
}

// Store the data from the char buffer and into a datafield
int lowerData(int* address, int offset, char* buff, int item_size)
{
    int temp;
    char* ptr = buff;
    int* position = address;
    for (char c = *ptr; c; c=*++ptr) {
        temp = c - '0';
        *position = temp;
        position += item_size;

    }
    return 1;

}

// int main() {
//     int limit = 10;
//     int item_size = sizeof(int);
//     int offset = 0;
//     int* datafield = calloc(limit, item_size);
//     int* datafieldlimit = dataFieldLimit(datafield, item_size, limit);

//     int temp = insertFirstRelative(datafield, 1, item_size,offset,datafieldlimit);
//     if (temp < 0) exit(1);
//     offset = temp;

//     temp = insertFirstRelative(datafield, 3, item_size,offset,datafieldlimit);
//     if (temp < 0) exit(1);
//     offset = temp;

//     temp = insertFirstRelative(datafield, 2, item_size,offset,datafieldlimit);
//     if (temp < 0) exit(1);
//     offset = temp;

//     char* buffer = calloc(20, sizeof(char));
//     int res = liftData(buffer, datafield, item_size, offset, 0);
//     if (res < 0) exit(1);

//     int* datafield2 = calloc(limit, item_size);
//     res = lowerData(datafield2, offset, buffer, datafieldlimit, item_size);
//     if (res < 0) exit(1);

//     printRelative(datafield2, item_size, offset);
// }
