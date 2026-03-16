#!/bin/bash

# 使用範例: ./check_files.sh c432

# 檢查輸入參數
if [ $# -ne 1 ]; then
    echo "Usage: $0 <testcase>"
    exit 1
fi

testcase=$1
filenames=("delay" "gate_info" "load" "path")

echo "=== Checking files for testcase: $testcase ==="

for name in "${filenames[@]}"; do
    golden="testcase/golden/${testcase}/golden_${testcase}_${name}.txt"
    public="./public_lib_${testcase}_${name}.txt"

    echo -n "Comparing ${name}: "

    if [ ! -f "$golden" ]; then
        echo "❌ Missing golden file: $golden"
        continue
    fi

    if [ ! -f "$public" ]; then
        echo "❌ Missing public file: $public"
        continue
    fi

    # 忽略尾部的換行符來比較檔案
    golden_trimmed=$(sed 's/[[:space:]]*$//' "$golden")
    public_trimmed=$(sed 's/[[:space:]]*$//' "$public")

    if diff -q <(echo "$golden_trimmed") <(echo "$public_trimmed") > /dev/null; then
        echo "✅ OK"
    else
        echo "❌ Different"
        # 顯示前幾行差異（可移除或註解掉）
        diff --color=auto "$golden" "$public" | head -n 10
    fi
done

echo "=== Check completed ==="