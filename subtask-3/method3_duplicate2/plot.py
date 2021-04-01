import pandas as pd
data = pd.read_csv('method3_04.txt',sep=' ',header=None)
data = pd.DataFrame(data)

import matplotlib.pyplot as plt
x = data[0]
y = data[1]
z = data[2]
plt.plot(x, y,'b--', linestyle = "solid", linewidth=0.6)
plt.plot(x, z, '--', color='orange', linestyle = "solid", linewidth = 0.6)

plt.show()
