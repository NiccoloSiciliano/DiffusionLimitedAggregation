import numpy as np
import matplotlib.pyplot as plt

if __name__ == "__main__":
    with open("../bash/times_linear_fully.txt", "r") as f :
        lines = f.readlines()
    graphic = dict()
    activeP = ""
    t = 1
    grid = 1
    particles = 1
    iterations = 1
    l = 0
    N = 10
    dimpertest  = 111
    with open("tests.txt", "w") as f:   
                    f.write("")
    while l < len(lines):
        if l%dimpertest == 0:
            activeP = lines[l][:-1]
            l += 1
        else:
            r = lines[l][:-1]
            graphic[r] = []
            l += 1
            for c in range(0,N):    
                print(l, lines[l][:-1]) 
                graphic[r].append(lines[l][:-1])
                l += 1
            if l%dimpertest == 0:
                newdict =dict()
                count =0 
                for k in graphic:
                    newdict[k] = 0
                    for v in graphic[k]:
                        newdict[k] += float(v)
                    newdict[k] = newdict[k]/N
                graphic = dict()
                with open("tests.txt", "a") as f:   
                    f.write(activeP + ":\n")
                    for k in newdict:
                        f.write(k + ": " + str(newdict[k]) + "\n")
    


