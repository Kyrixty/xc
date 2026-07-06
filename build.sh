if [[ ! -d build ]]; then
    mkdir build
fi

gcc -Iinclude/ -g3 src/*.c -o build/xc.exe
(( code = $? ))

if [[ $code -ne 0 ]]; then
    printf "\033[31mBuild failed!\033[0m\n"
    exit $code
fi
build/xc.exe