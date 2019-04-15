import numpy as np
import matplotlib
import matplotlib.pyplot as plt

scatter_32768 = np.array([2.564583, 2.754509, 1.472941, 0.803871, 0.904196, 3.762699, 3.751775])
calc_32768 = np.array([0.705935, 0.349013, 0.176863, 0.155272, 0.097594, 0.050689, 0.055023])
p = np.array([1, 2, 4, 8, 16, 32, 64])
serial_32768 = 0.715940
plt.plot(p, serial_32768 / calc_32768, "+")
plt.plot(p, serial_32768 / (scatter_32768 + calc_32768), "*")
plt.xlabel('p')
plt.ylabel('speedup')
plt.savefig("speedup_32768.png")
plt.cla()

