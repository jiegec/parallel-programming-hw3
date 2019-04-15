Exercise 3.11(d)
===================================

## 实现目标

p个进程，每个进程分别计算前缀和，再合起来。

## 实现过程

### 分别生成随机数

每个进程生成自己的数据和前缀和。

```cpp
    srand(my_rank + time(NULL));

    int local_prefix[10];
    int local_data[10];
    for (int i = 0;i < 10;i++) {
        local_data[i] = rand() % 10;
        local_prefix[i] = local_data[i];
        if (i > 0) {
            local_prefix[i] += local_prefix[i-1];
        }
    }
```

### 使用 `MPI_Scan` 获得前面进程的前缀和 

传入当前10个数的和，然后进行 `MPI_Scan` ，得到前面数据的和

```cpp
    int prefix_before;

    MPI_Scan(&local_prefix[10-1], &prefix_before, 1, MPI_INT, MPI_SUM, comm);

    prefix_before -= local_prefix[10-1];
```

### 更新本身的前缀和，并传递给 0 号进程

通过 `MPI_Gather` 发送给 0 号进程

```cpp
    for (int i = 0;i < 10;i++) {
        local_prefix[i] += prefix_before;
    }

    MPI_Gather(local_prefix, 10, MPI_INT, prefix, 10, MPI_INT, 0, comm);
    MPI_Gather(local_data, 10, MPI_INT, data, 10, MPI_INT, 0, comm);
```

### 0 号进程输出结果并验证

```cpp
    if (my_rank == 0) {
        printf("Data: ");
        for (int i = 0;i < 10 * comm_sz;i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
        printf("Prefix: ");
        for (int i = 0;i < 10 * comm_sz;i++) {
            printf("%d ", prefix[i]);
        }
        printf("\n");
        printf("Expected: ");
        int temp = 0;
        for (int i = 0;i < 10 * comm_sz;i++) {
            temp += data[i];
            printf("%d ", temp);
        }
    }
```

## 运行方法和测试结果

运行：

```cpp
$ make prog3.11d
$ mpirun -n 4 ./prog3.11d
```

输出：

```
Data: 2 9 6 0 5 3 5 1 6 1 9 8 2 8 8 5 9 2 2 3 6 7 5 9 8 7 3 3 5 5 3 9 1 7 1 2 7 1 1 7 
Prefix: 2 11 17 17 22 25 30 31 37 38 47 55 57 65 73 78 87 89 91 94 100 107 112 121 129 136 139 142 147 152 155 164 165 172 173 175 182 183 184 191 
Expected: 2 11 17 17 22 25 30 31 37 38 47 55 57 65 73 78 87 89 91 94 100 107 112 121 129 136 139 142 147 152 155 164 165 172 173 175 182 183 184 191 
```

