if [[ ! -d dist/test ]]; then
    mkdir -p dist/test
fi

passed=0
total=0

while IFS= read -r -d '' file; do
    filename=$(basename "$file")
    filename_no_ext="${filename%.*}"
    
    echo -e "===== TEST" "$filename"

    if gcc -Wall -Iinclude/ -O0 "$file" -o "dist/test/$filename_no_ext.exe" && "dist/test/$filename_no_ext.exe"; then
        (( passed++ ))
        status="\033[32m Pass\033[0m"
    else
        status="\033[31m Fail\033[0m"
    fi
    
    echo -e "$status"
    (( total++ ))
done < <(find ./tests -type f -print0)

echo "----------------------------"
echo -e "\t$passed/$total passed"