# importing the required module
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

x_axis = "Grid"
y_axis = "SpeedUp"
# x axis values
x = [200, 400, 600, 800, 1000, 1200,1400, 1600, 1800, 2000]
# corresponding y axis values
with open("tests_linear_fully.txt", "r") as f:
    lines = f.readlines()
graph = dict()
l = 0
linear_time = 0
while l < len(lines):
    if lines[l].split(" ")[0] == x_axis:
        l+=1
        linear_time = float(lines[l].split(":")[1][:-1])
        l+=1
        for i in range(10):
            p = float(lines[l].split(":")[0].split(" ")[1])
            if lines[l].split(":")[0] not in graph:
                graph[lines[l].split(":")[0]] = [linear_time/(float(lines[l].split(":")[1][:-1])*p)]
            else:
                graph[lines[l].split(":")[0]].append(linear_time/(float(lines[l].split(":")[1][:-1])*p))
            l += 1
    else:
        l += 1


ypt = []
yomp = []
for i in range(5):
    ypt.append(graph["./pthread/main_linear_fully.o "+str(pow(2,i+1))]) 
for i in range(5):
    yomp.append(graph["./omp/main_linear_fully.o "+str(pow(2,i+1))]) 
linestyles = ["-", "--", ":", "-."]
# plotting the points 
for i in range(4):
    plt.plot(x, ypt[i], color = "#f75e25", linestyle=linestyles[i])
    plt.plot(x, yomp[i], color = "#0095b6", linestyle=linestyles[i])
for k in range(4):
    plt.scatter(x,ypt[k],s=20, c='#f75e25')
    plt.scatter(x,yomp[k],s=20, c='#0095b6')
red_patch = plt.Line2D([], [], color='#f75e25', marker='o', linestyle='None',
                          markersize=5, label='pthread')
blue_patch = plt.Line2D([], [], color='#0095b6', marker='o', linestyle='None',
                          markersize=5, label='omp')
black_patch = []
for i in range(4):
    black_patch.append(plt.Line2D([0] ,[1], color='black',label= str(pow(2, i+1))+" threads", linestyle = linestyles[i]))

plt.legend(handles=([red_patch, blue_patch] + black_patch),loc="upper left")
#axis[0].xaxis.set_ticks(np.arange(min(x), max(x) + 200,x[1] - x[0] ))
plt.xticks(np.arange(min(x), max(x) + 200,x[1] - x[0] ))
#axis[0].legend(loc="upper left")

#axis[0].set_title("Pthread")
plt.title("PTHREAD and OMP (" +x_axis+")")
# naming the x axis
#axis[0].set_xlabel(x_axis)
plt.ylabel(y_axis)
# naming the y axis
#axis[0].set_xlabel(x_axis)
plt.xlabel(x_axis)
  
# function to show the plot
plt.show()