#!/usr/bin/env python3

import os
import subprocess


def compile_and_run_test(test_file):
    # 生成汇编文件
    compile_result = subprocess.run(
        ["../myComp", test_file], stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    test_name = os.path.basename(test_file).split(".")[0]

    # 如果编译失败, 保存标准输出中的错误信息
    if compile_result.returncode != 0:
        output = compile_result.stderr.decode("utf-8").strip()
    # 否则生成可执行文件
    else:
        subprocess.call(["cc", "-o", "out", "out.s", "../lib/printint.c"])

        # 运行测试
        output = subprocess.check_output(["./out"]).decode("utf-8").strip()

    # 比较实际输出与预期输出
    expected_output_file = "./outputs/out." + test_name + ".c"
    with open(expected_output_file, "r") as f:
        expected_output = f.read().strip()

    if output == expected_output:
        print(f"test {test_name} passed")
    else:
        print(f"test {test_name} failed")
        print("wrong output:")
        print(output)
        print("expected output:")
        print(expected_output)

        # 立即退出
        exit(1)

    # 删除生成的文件
    if os.path.exists("out"):
        os.remove("out")
    if os.path.exists("out.s"):
        os.remove("out.s")


def main():
    # 找到所有的测试文件
    test_files = [
        filename
        for filename in os.listdir("codes")
        if filename.startswith("input") and filename.endswith(".c")
    ]

    for test_file in test_files:
        compile_and_run_test(os.path.join("codes", test_file))


if __name__ == "__main__":
    main()
