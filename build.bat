@echo off
echo start building...

gcc ^
  ./core/Vexel.c ^
  ./core/v-heap.c ^
  ./objects/v-data.c ^
  ./objects/v-list.c ^
  -o Vexel

echo build complete!
