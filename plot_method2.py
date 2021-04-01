import csv
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 22})
import os

all_files = os.listdir("method2/");
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
        file = "method2/" + file
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
            # print(param)
            fx = float(param[:4])
            fy = float(param[5:])
            # print(fx)
            # print(fy)
            parameter.append([fx, fy, fx*fy])

data = []
data.append(utility)
data.append(runtime)
data.append(parameter)


# comment out one of the plots before running

# PLOTS UTILITY VS PARAMETER (WHERE PARAMETER IS fx*fy)
plt.plot([row[2] for row in data[2]], data[0], 'bo-', label="Utility VS Parameter")
for x, y in zip(data[2], data[0]):
    fx = x[0]
    fy = x[1]
    # print(fx)
    # print(fy)
    label = "{:.2f} X {:.2f}".format(fx, fy)
    plt.annotate(label,
                (x[2],y),
                textcoords="offset points",
                xytext=(0, 10),
                ha='center')

plt.xlabel('Parameter')
plt.ylabel('Utility')
plt.title("Method2: Utility VS Parameter")
plt.show()

# # PLOTS UTILITY VS RUNTIME
# plt.plot(data[1], data[0], 'ro-', label="Utility VS Runtime")
#
# for i in range(0, len(data[1])):
#     fx = data[2][i][0]
#     fy = data[2][i][1]
#     label = "{:.2f} X {:.2f}".format(fx, fy)
#     plt.annotate(label,
#                  (data[1][i], data[0][i]),
#                  textcoords="offset points",
#                  xytext=(0, 5),
#                  ha='center')
#
# plt.xlabel('Runtime')
# plt.ylabel('Utility')
# plt.title("Method2: Utility VS Runtime")
# plt.show()
