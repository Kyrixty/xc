if [[ ! -d dist ]]; then
    mkdir build
fi

time gcc -Iinclude/ -g3 src/*.c -o dist/xc.exe
(( code = $? ))

if [[ $code -ne 0 ]]; then
    printf "\033[31mBuild failed!\033[0m\n"
    exit $code
fi
dist/xc.exe