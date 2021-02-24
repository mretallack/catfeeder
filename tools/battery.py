
import matplotlib
matplotlib.use('GTK3Agg')

import matplotlib.pyplot as plt 

# set the points here
# use y for voltage
# and x for raw value
y=[5.254, 5.96]
x=[164, 193]

m=(y[1]-y[0])/(x[1]-x[0])

c= y[0] - (x[0] * m)


print("M="+str(m))
print("C="+str(c))

rawMeasurements=[]
vMeasurements=[]

for i in range(255):
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


