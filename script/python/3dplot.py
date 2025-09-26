import numpy as np
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ax = plt.figure().add_subplot(projection='3d')

    # Plot a sin curve using the x and y axes.

    # Plot scatterplot data (20 2D points per colour) on the x and z axes.
    colors = ('r', 'g', 'b', 'k')
    coord = []
    with open("crystal.txt","r") as f:
        coords = f.readlines()
    x = []
    y = []
    z = []
    c_list = []
    i = 0 
    for c in coords:
        xyz = c.split(" ")
        if i == 0:
            dim = int(xyz[0])
        else: 
            x.append(int(xyz[0]))
            y.append(int(xyz[1]))
            z.append(int(xyz[2])) 
            if xyz[3] == "c\n":
                c_list.append((0,0,1,0.4))
            else:
                c_list.append((1,0,0,0))
        i+=1
    
    
    # By using zdir='y', the y value of these points is fixed to the zs value 0
    # and the (x, y) points are plotted on the x and z axes.
    ax.scatter(x, z, y, zdir='y', c=c_list, label='crystal')

    # Make legend, set axes limits and labels
    ax.legend()
    
    ax.set_xlim(0, dim)
    ax.set_ylim(0, dim)
    ax.set_zlim(0, dim)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    # Customize the view angle so it's easier to see that the scatter points lie
    # on the plane y=0
    ax.view_init(elev=20., azim=-35, roll=0)

    plt.show()
