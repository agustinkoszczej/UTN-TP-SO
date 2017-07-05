#!usr/bin/sh

clear

cd utils
cd Debug
make clean
make all
cd ..
cd ..

cd consola
cd Debug
make clean
make all
cd ..
cd ..

cd cpu
cd Debug
make clean
make all
cd ..
cd ..

cd filesystem
cd Debug
make clean
make all
cd ..
cd ..

cd kernel
cd Debug
make clean
make all
cd ..
cd ..

cd memoria
cd Debug
make clean
make all
cd ..
cd ..

echo ""
echo "Compilation completed."
