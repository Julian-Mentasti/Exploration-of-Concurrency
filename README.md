# Exploration-of-Concurrency

## Summary of work:

Thought this seminar I got a better grasp of the underlying tools that are used for managing and implementing distributed systems. My coursework project was researching and testing if the MPI library could enhance different common tools and libraries to enable their use in very efficient MPI programmes.

I first began sending JSON files though processes using MPI. I experimented with two libraries: jasmine and json-c. Though my attempts I was able to send data using both libraries. However, after further experimenting and attempting to create a simple 'plug-and-play' library it became clear to me that since MPI nor the external libraries are built into the C language I can't make a library "see" what a structure from those libraries looks like, thus I am unable to serialize it. Hence if I need to send complex datatypes, I have to explicitly define its layout. However, by pulling the entirety of the structure into a known data buffer I could transfer the JSON from one process to another.

My next task was to be able to send protobuf data in between processes. I initially looked at the protoc-c library. However, it was rather complicated to build and use when it came to linking the generated proto files. The proto-c compiler from my understanding builds upon Google's proto compiler, but uses a slightly different linking structure which became an issue when compiling a program using the mpicc compiler, which is a gcc wrapper. This lead me to look for other protobuf implementations in C, one of them being the Nanopb library. It uses google's proto compiler and with this I was able to send a uint8_t protoc buffer though MPI.

Then I proceeded to send more complex datatype to send XDR data, this required me to send rather complex structs which is possible using MPI_Pack or custom MPI_Structs, although this required a large amount of lines of code to be added. Hence I worked to create a 'simple' library that could add the necessary pieces at compiler time using COG but I was unable to generate the needed lines due to the complexity of the custom structs. I would have liked to spend more time and build an external 'compiler' that given these complex structs it would generate all the code and use that for XDR.

Lastly I focused on sending complex data structures though a method with little to no overhead. In theory if one wanted they could have a linked list as a set of continuous data and an offset, an each value could obtained by relative addressing. Thus there would be no need for pointers or other 'linking' tools such as pointers. I created a 'rellist' library that allowed me to create these data structures as bits and 'lift' it so it could be sent though MPI. On the other end I could then lower it and interact with it using relative addressing. This project could be extended upon to support non continuous chunks or pages of memory, thus supporting very large data structures that need to be in non-continuous memory address to fit. When testing this method's approach it was clear that it was significantly more efficient, in terms of memory, than a standard pointer based linked list. However, computationally wise it was not significantly faster, in fact when accounting for the data 'lifting' and 'lowering' it was more computationally expensive. It should be considered that the pointer based linked list would need to be packed in order to be sent though MPI, which would add computational complexity.

Overall, it was a very interesting set of projects that required me to dive into several complex libraries so I could utilize them together. Looking back I spent a large amount of time understanding how data could be sent though MPI and then how to convert data from different libraries into default MPI data types. If I had more time I would spend it making a more robust and reliable method to send protobuf data though MPI and then creating a compiler for handle sending XDR data such as trees. I would also look into expand the 'rellist' library to support different data types and as mentioned above support non-continuous data chunks.

## Files:

You can see my weekly work though each directoy, in my `overall` directory has the relevant programs compiled.

- json_exploration: working with jsons
- mpi_created_solutions: Packing and Unpacking, Custom Structs, MPI test programs
- send_raw: `rellist` library for a linked list with little overhead.
- protobuf: protobuf MPI code built upon the nanopb library.

## Libraries used in the coruse
- MPI
- Protoc
- Nanopb
- libuv
- libaco
- JSON-C
- Jasmine
- XDR
