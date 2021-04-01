import csv
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 22})
import os

all_files = os.listdir("threads/");
# print(all_files)
txt_files = list(filter(lambda x: x[-4:] == ".txt", all_files))
print(txt_files)
txt_files.sort()
num_files = len(txt_files)

error = []
utility = []
runtime = []
parameter = []

with open('baseline.txt', 'r') as base:
    base_data = list(csv.reader(base, delimiter=' '))
    n = len(base_data)


    for file in txt_files:
        erro = 0
        file = "threads/" + file
        with open(file) as var_method:
            var_data = list(csv.reader(var_method, delimiter=' '))
            print(var_data[-1])
            time = var_data[-1][2]
            runtime.append(float(time))
            param = file[16:-4]
            parameter.append(param)
            # print(param)

data = []
# data.append(utility)
data.append(runtime)
data.append(parameter)


plt.plot(data[1], data[0], 'ro-', label="Runtime VS Parameter")

plt.xlabel('Parameter')
plt.ylabel('Runtime')
plt.title("Method4: Runtime VS Parameter")
plt.show()
