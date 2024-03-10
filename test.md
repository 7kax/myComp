# Test 相关

## 执行测试的方式

- `test`文件夹中有若干测试文件, 命名为`test*.c`
- 测试脚本`test.py`在命令行中运行`./myComp ./test*.c`, 同目录下将生成`test*.s`文件
- 运行`cc -o test*.out test*.s ../lib/printint.c`生成可执行文件`test*.out`
- 运行`./test*.out`执行测试
- 程序预期的输出在`out.test*.c`中, 与实际输出进行比较
- `test.py`会输出测试结果
