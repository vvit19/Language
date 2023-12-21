cd frontend
make
./front ../examples/$1.vit
cd ..

cd middleend
make
./middle
cd ..

cd backend
make
./back ../examples/$1.asm
cd ..

cd processor/asm
make
./run ../../examples/$1.asm ../../examples/$1.bin
cd ../spu
make
./run ../../examples/$1.bin
cd ../..
