import pandas as pd
data = pd.read_csv('time.txt',sep=' ',header=None)
data = pd.DataFrame(data)

import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 22})
x = data[0]
y = data[1]
z = data[2]
plt.plot(x, y,'ro-', label = "CPU Clock Runtime")
plt.plot(x, z,'go-', label = "Wall Time")
plt.xlabel('Parameter (Num of threads)')
plt.ylabel('Time')
plt.title("Method4: Time VS Parameter")
plt.legend()

plt.show()
