# This command once was able to cross compile a working windows binary. But never again. Just keeping it around for future reference.

x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++ -static -std=c++2a -pedantic -fomit-frame-pointer -O3 -L/usr/x86_64-w64-mingw32 -o signalbackup-tools.exe */*.cc *.cc -lcryptopp -lsqlite3
