rm out/*
protoc -I=.:/usr/local/include --cpp_out=out compile.proto 
cp out/compile.pb.cc out/compile.pb.cpp
