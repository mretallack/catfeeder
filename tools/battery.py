
import matplotlib
matplotlib.use('GTK3Agg')

import matplotlib.pyplot as plt 

# set the points here
# use y for voltage
# and x for raw value

y=[5.254, 5.520, 5.66, 5.77, 5.85, 5.96]
x=[164, 179, 182, 187, 189, 193]

f=0
l=4

m=(y[l]-y[f])/(x[l]-x[f])

c= y[f] - (x[f] * m)


print("M="+str(m))
print("C="+str(c))

rawMeasurements=[]
vMeasurements=[]

for i in range(160,200):
	rawMeasurements.append(i)
	res=(i*m) +c
	vMeasurements.append(res)


plt.plot(rawMeasurements, vMeasurements) 

plt.scatter(x, y) 

# naming the x axis 
plt.xlabel('Raw byte reading') 
# naming the y axis 
plt.ylabel('Battery Voltage') 
  
# giving a title to my graph 
plt.title('Battery Levels') 
  
# function to show the plot 
plt.show() 


