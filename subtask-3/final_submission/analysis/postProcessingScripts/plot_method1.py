import csv
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 22})
import os

all_files = os.listdir("method1/");
# print(all_files)
txt_files = list(filter(lambda x: x[-4:] == ".txt", all_files))
txt_files.sort()
# print(txt_files)
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
        file = "method1/" + file
        with open(file) as var_method:
            var_data = list(csv.reader(var_method, delimiter=' '))
            # print(var_data)
            for row in range(0, len(var_data)-1):
                var_qd = var_data[row][1]
                base_qd = base_data[row][1]
                erro += abs(float(var_qd) - float(base_qd))
            error.append(erro)
            utility.append(1/erro)
            time = var_data[-1][2]
            runtime.append(float(time))
            param = file[16:-4]
            parameter.append(float(param))

data = []
data.append(utility)
data.append(runtime)
data.append(parameter)

# # data.sort(key = lambda x : x[1])

# comment one of the below before running

# # PLOTS UTILITY VS PARAMETER
# plt.plot(data[2], data[0],'bo-', label="Utility VS Parameter")
# plt.xlabel('Parameter')
# plt.ylabel('Utility')
# plt.title("Method1: Utility VS Parameter")
# plt.show()

# PLOTS UTILITY VS RUNTIME
plt.plot(data[1], data[0],'ro-', label="Utility VS Runtime")
plt.xlabel('Runtime')
plt.ylabel('Utility')
plt.title("Method1: Utility VS Runtime")
plt.show()



# fig, (ax1, ax2) = plt.subplots(1, 2)
# fig.suptitle("Utility vs Runtime                                                                Utility vs Parameter")
# ax1.plot(data[1], data[0])
# ax2.plot(data[2], data[0])
# # plt.setp(ax1[0])
# plt.show()
