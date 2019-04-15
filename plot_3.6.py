import numpy as np
import matplotlib
import matplotlib.pyplot as plt

scatter_32768 = np.array([2.551954, 1.489790, 0.583201, 4.831784])
calc_32768 = np.array([0.707172, 0.192226, 0.109212, 0.027418])
p = np.array([1, 4, 16, 64])
serial_32768 = 0.828762
plt.plot(p, serial_32768 / calc_32768)
plt.plot(p, serial_32768 / (scatter_32768 + calc_32768))
plt.xlabel('p (n = 32768)')
plt.ylabel('speedup')
plt.savefig("speedup_32768_3.6.png")
plt.cla()

scatter_64 = np.array([0.005826, 0.016571, 0.0330780, 1.206173, 4.831784])
calc_64 = np.array([0.000209, 0.000278, 0.002585, 0.010320, 0.027418])
n = np.array([512, 2048, 8192, 16384, 32768])
serial_64 = np.array([0.000134, 0.002837, 0.041194, 0.282623, 0.828762])
plt.plot(n, serial_64 / calc_64)
plt.plot(n, serial_64 / (scatter_64 + calc_64))
plt.xlabel('n (p = 64)')
plt.ylabel('speedup')
plt.savefig("speedup_64_3.6.png")
plt.cla()


