#ifndef rellist_h_INCLUDED
#define rellist_h_INCLUDED
int* dataFieldLimit(int* start_address, int item_size, int item_numbers);

int insertFirstRelative(int* address, int value, int item_size, int offset, int* limit);

int deleteFristRelative(int item_size, int offset);

void printRelative(int* address, int item_size, int offset);

int lengthRelative(int item_size, int offset);

int liftData(char* buff, int* address, int item_size, int data_offset, int buff_offset);

int append(char* buff, char c, int position, int limit);

int lowerData(int* address, int offset, char* buff, int item_size);
#endif // rellist_h_INCLUDED

