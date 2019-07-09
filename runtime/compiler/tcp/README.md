This extension currently does not have proto building integrated with the JVM build.

This means that the protobuf template (the compile.proto file) needs to be compiled into the .h .cc and .cpp files
**before** the JVM build.

To do this use `./compileproto.sh`.

The resulting files are placed in the `out` directory, which is the expected location for the JVM to know about these files when building.