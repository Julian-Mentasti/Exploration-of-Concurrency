# Sending JSON Struct thought MPI

﻿The json-c library allows for simple json to char[] conversion which can later be sent though MPI quite easily without needing to pack complex structs such as Jasmine's `json_object`. This is a simpler more elegant solution since the size of the structs may vary depending on the size of the JSON that has been marshalled.

I also experimented with the Jasmine library, this library used more complex stucts inorder to serialise a JSON file, however this made it
more channeling to send thought MPI. This meant I had to either utilise `MPI_Struct` or `MPI_Pack`/`MPI_Unpack`, which are routines are designed
 to provide compatibility for libraries that send data though a noncontinuous buffer. A message can be received in several parts, where the receive
 operation done on a later part can depend on the previous received part.

json-c is able to serialise the entire JSON datatype in an MPI_Char datatype so it makes it very easy to pack and unpack a JSON datastructure though MPI.

### MPI_Struct exploration with json-c's json_object

`json_object` exists in `json_object_private.h` and its dyanimc datastructure that may change size depending on the JSON being unmarshalled.

`json_obect` struct:
```C
struct json_object
{
  enum json_type o_type;
  uint32_t _ref_count;
  json_object_private_delete_fn *_delete;
  json_object_to_json_string_fn *_to_json_string;
  struct printbuf *_pb;
  union data {
    json_bool c_boolean;
    double c_double;
    int64_t c_int64;
    struct lh_table *c_object;
    struct array_list *c_array;
    struct {
	union {
		/* optimize: if we have small strings, we can store them
		 * directly. This saves considerable CPU cycles AND memory.
		 */
		char *ptr;
		char data[LEN_DIRECT_STRING_DATA];
	} str;
        int len;
    } c_string;
  } o;
  json_object_delete_fn *_user_delete;
  void *_userdata;
};
```

`MPI_Struct` requires me to be able to create a data structure that had the number of different data types and their respective sizes. It became rather challenging
 to create a simple solution that did not reply on explicitly defining each data structure and their size. Thus I opted to convert everything to `MPI_Char.h`.

## Testing the example

To build
```
make build
```

To run
```
make run
```
