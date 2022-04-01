# Reflexion Colin van Loo

I hadn't worked with microprocessors before, and had almost no experience using
C or C++. Luckily there were many good examples explaining how to use and
interact with the different sensors on the IoTKit. I also learned a lot about
C(++) from searching "C Best Practices" on lobste.rs and other places on the
Internet. I've run into many problems with the IoTKit, Sensors not working
correctly -- even when using the example code, -- setting up the network
failing with weird error codes and no further explanation. (After long
searching, I found an error description hidden a way in the libraries source
code.) In the end I managed to solve all those problems and learned many new
things. Programming the MCU was fun, but, besides of `print`s, hard to debug.
That's why I made sure to keep the code as simple as possible. Easier was
programming the webservice, since I could use Go, a language I already have
experience with.
