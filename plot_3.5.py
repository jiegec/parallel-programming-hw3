import numpy as np
import matplotlib
import matplotlib.pyplot as plt

scatter_32768 = np.array([2.564583, 2.754509, 1.472941, 0.803871, 0.904196, 3.762699, 4.861116])
calc_32768 = np.array([0.705935, 0.349013, 0.176863, 0.155272, 0.097594, 0.050689, 0.055023])
p = np.array([1, 2, 4, 8, 16, 32, 64])
serial_32768 = 0.715940
plt.plot(p, serial_32768 / calc_32768)
plt.plot(p, serial_32768 / (scatter_32768 + calc_32768))
plt.xlabel('p (n = 32768)')
plt.ylabel('speedup')
plt.savefig("speedup_32768.png")
plt.cla()

scatter_64 = np.array([0.005673, 0.027237, 0.344984, 1.238004, 4.861116])
calc_64 = np.array([0.000421, 0.000858, 0.003288, 0.008784, 0.029271])
n = np.array([512, 2048, 8192, 16384, 32768])
serial_64 = np.array([0.000166, 0.002786, 0.036893, 0.280732, 0.715940])
plt.plot(n, serial_64 / calc_64)
plt.plot(n, serial_64 / (scatter_64 + calc_64))
plt.xlabel('n (p = 64)')
plt.ylabel('speedup')
plt.savefig("speedup_64.png")
plt.cla()


