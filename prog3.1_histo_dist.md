Program Assignments 3.1
===================================

## 实现目标

随机生成数据，分成等距的若干个 bin ，并输出直方图。

## 实现过程

### 实现 `Which_bin`

循环每个 bin ，判断是否落在范围内。

```cpp
int Which_bin(float data, float bin_maxes[], int bin_count, 
      float min_meas) {
   if (min_meas <= data && data < bin_maxes[0]) {
      return 0;
   }
   for (int i = 1;i < bin_count;i++) {
      if (bin_maxes[i-1] <= data && data < bin_maxes[i]) {
         return i;
      }
   }
   // impossible
   return -1;
}  /* Which_bin */
```

### 实现 `Find_bins` 

对每一个分配到本进程的数据，统计到指定的 bin 中。最后通过 `MPI_Reduce` 合并结果到 0 号进程。

```cpp
void Find_bins(
      int bin_counts[]      /* out */, 
      float local_data[]    /* in  */,
      int loc_bin_cts[]     /* out */, 
      int local_data_count  /* in  */, 
      float bin_maxes[]     /* in  */,
      int bin_count         /* in  */, 
      float min_meas        /* in  */, 
      MPI_Comm comm){
   /* Use a for loop to find bins, the statement in the loop can be:
        bin = Which_bin(local_data[i], bin_maxes, bin_count, min_meas);
      Then, calculate the global sum using collective communication.
   */
   for (int i = 0;i < local_data_count;i++) {
      int bin = Which_bin(local_data[i], bin_maxes, bin_count, min_meas);
      loc_bin_cts[bin] ++;
   }
   MPI_Reduce(loc_bin_cts, bin_counts, bin_count, MPI_FLOAT, MPI_SUM, 0, comm);
   
}  /* Find_bins */
```

## 运行方法和测试结果

运行：

```cpp
$ make prog3.1_histo_dist
$ mpirun -n 4 ./prog3.1_histo_dist
```

输出：

```
Enter the number of bins
4
Enter the minimum measurement
0
Enter the maximum measurement
100
Enter the number of data
40
0.000-25.000:   XXXXXXXXX
25.000-50.000:  XXXXXXXXX
50.000-75.000:  XXXXXXXXXX
75.000-100.000: XXXXXXXXXXXX
```

各个 bin 的数量基本一致。