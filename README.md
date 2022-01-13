# quetest
Demo of a distributed glm transformation framework I wrote about 5 years ago

Recently interviewed for a plumb job and they denied me 'cause I didn't go into enough detail or some shit about this project I did over 5 years ago so I'm re-creating it from memory and making some improvements.

Point here is to demonstrait a method of distrubuting complex mathematical operations among a group of hosts to lesson the computation time. A file(s) (in this case an stl model) are put into an input queue, operated on (in this case sliced for 3d printing), and put into an output queue. The system will use the 0MQ API to 1) enumerate the available slicers, 2) do the trransformations on sections of the model to be lazed, 3) re-assemble the model into a printable model file.

In this greatly simplified demo I will simulate a queue master and a slicer client on the same host, but I anticipate being able to easily move the client nodes out to actual hosts after I get the main demo working.

Update: Still getting the ouput of the slicer to get turned into GCode.
