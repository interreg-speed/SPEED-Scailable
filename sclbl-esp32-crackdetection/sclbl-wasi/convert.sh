# !/bin/bash

#==============================================================================================#
#                optimize                                                                      #
#==============================================================================================#

wasm-strip sclbl.wasm
wasm-opt -O3 -o sclbl.wasm sclbl.wasm

#==============================================================================================#
#                write wasm to c header file                                                   #
#==============================================================================================#

xxd -i sclbl.wasm > sclbl.wasm.h

# prepend const - allowing esp32 compiler to treat it as such.

cat <<-EOF > sclbl.wasm.h
  $(echo const ) $(cat sclbl.wasm.h)
EOF
