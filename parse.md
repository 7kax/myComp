# parse 相关

## 运算符优先级

| 运算符                              | 左优先级 | 右优先级 |
| ----------------------------------- | -------- | -------- |
| `[]`, `()`                          | 29       | 30       |
| `-`, `++`, `--`, `*`, `&`, `!`, `~` | 28       | 27       |
| `*`, `/`, `%`                       | 25       | 26       |
| `+`, `-`                            | 23       | 24       |
| `<<`, `>>`                          | 21       | 22       |
| `<`, `<=`, `>`, `>=`                | 19       | 20       |
| `==`, `!=`                          | 17       | 18       |
| `&`                                 | 15       | 16       |
| `^`                                 | 13       | 14       |
| `\|`                                | 11       | 12       |
| `&&`                                | 9        | 10       |
| `\|\|`                              | 7        | 8        |
| `=`                                 | 6        | 5        |

目前只有二元运算符在解析过程中会考虑优先级, 一元运算符作特殊处理.
