#!/usr/bin/env python3

import os
import subprocess
from pathlib import Path

all_correct = True


def cleanup():
    # 删除生成的文件
    if os.path.exists("out"):
        os.remove("out")
    if os.path.exists("out.s"):
        os.remove("out.s")


def compile_and_run_test(test_file: Path):
    # 生成汇编文件
    compile_result = subprocess.run(
        ["../myComp", test_file], stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    test_name = test_file.stem
    output_path = test_file.parent.parent.joinpath("outputs")

    # 如果编译失败, 保存标准输出中的错误信息
    if compile_result.returncode != 0:
        output = compile_result.stderr.decode("utf-8").strip()
    # 否则生成可执行文件
    else:
        subprocess.call(["cc", "-o", "out", "out.s", "../lib/printint.c"])

        # 运行测试, 如果测试文件有输入, 则将输入重定向到测试文件
        # output = subprocess.check_output(["./out"]).decode("utf-8").strip()
        if output_path.joinpath(test_name + ".in").exists():
            with open(output_path.joinpath(test_name + ".in"), "r") as f:
                output = (
                    subprocess.check_output(["./out"], stdin=f).decode("utf-8").strip()
                )
        else:
            output = subprocess.check_output(["./out"]).decode("utf-8").strip()

    # 比较实际输出与预期输出
    expected_output_file = test_file.parent.parent.joinpath("outputs").joinpath(
        test_name + ".out"
    )
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
        global all_correct
        all_correct = False

    print()


def main():
    # 所有的测试类型
    test_types = [
        "algorithm",  # 测试编译器是否能正确编译算法
        "function",  # 测试编译器是否实现了指定的功能
        "error",  # 测试编译器是否能正确处理错误
    ]

    for test_type in test_types:
        print(f"running {test_type} tests")

        # 找到所有的测试文件
        test_files = list(Path(f"{test_type}").joinpath("codes").rglob("*.c"))

        print(f"found {len(test_files)} test files in {test_type} codes")

        print()

        for test_file in test_files:
            compile_and_run_test(test_file)

    cleanup()

    if all_correct:
        print("ALL TESTS PASSED!!!")
    else:
        print("SOME TESTS FAILED!!!")


if __name__ == "__main__":
    main()
