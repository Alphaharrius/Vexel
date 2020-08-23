@echo off
echo start building...

gcc ^
  ./core/Vexel.c ^
  ./core/v-heap.c ^
  ./core/v-crypto.c ^
  ./objects/v-data.c ^
  ./objects/v-list.c ^
  -o Vexel

echo build complete!
