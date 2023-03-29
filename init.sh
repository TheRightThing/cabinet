git submodule init && git submodule update

mkdir build/ncurses
PREFIX="$PWD/build/ncurses"
cd deps/ncurses
sh configure --prefix "$PREFIX"
make install
