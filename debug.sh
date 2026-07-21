if [[ ! -d dist/debug ]]; then
    mkdir -p dist/debug
fi

time gcc -O0 -Wall -Iinclude/ -g3 src/*.c -o dist/debug/xc.exe
(( code = $? ))

if [[ $code -ne 0 ]]; then
    printf "\033[31mBuild failed!\033[0m\n"
    exit $code
fi
printf "\033[32mDebug Build successful!\033[0m\n"