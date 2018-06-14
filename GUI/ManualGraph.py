import ftplib
import csv
import datetime
from matplotlib import pyplot as plt
import os
import time
import re

dictOld = dict()
dictNew = dict()
dictRange = dict()
plt.ion()


########################## FUNCTIONS ######################################

# Function to get file from the ftp server and save locally
def getfile(ftp, filename):
    try:
        ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
    except:
        print("Error")


# Function to read CSV file and create a dictionary with key as time and parameter as value
def readfile(filename, parameter, dictionary):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            time = row['Time']
            dictionary[time] = float(row['{}'.format(parameter)])
            

# function to read the header names of the file
def readheader(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        print("Enter one of the following parameters:")
        headers = list(next(reader))
        for index, item in enumerate(headers):
            if item == 'Time':
                del(headers[index])
        print(headers)


# Function to send data file to the FTP server
def appendfile(ftp, filename):
    ftp.storbinary("APPE " + filename, open(filename, 'rb'))


# Function to send file to the FTP server
def uploadfile(ftp, filename):
    ftp.storbinary("STOR " + filename, open(filename, 'rb'))


# merging 2 dicitionaries
def merge_two_dicts(x, y):
    z = x.copy()   # start with x's keys and values
    z.update(y)    # modifies z with y's keys and values & returns None
    return z


# function to specify the range to be graphed
def getRange(startDate, endDate, dictionary, dictionaryRange):
    startFound = False
    endFound = False
    for key in sorted(dictionary):
        if re.search(startDate, '{}'.format(key)):
            startFound = True
        if  not re.search(endDate, '{}'.format(key)) and endFound == True:
            break
        if startFound == True:
            dictionaryRange[key] = dictionary[key]
        if re.search(endDate, '{}'.format(key)):
            dictionaryRange[key] = dictionary[key]
            endFound = True


# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, refresh, startDate, endDate, paramType):
    xlabel = 'Date and Time'
    if paramType == "Level":
        title = "Water Level"
        ylabel = "Level (m)"
        ymax = 0.40
        ymin = -0.005
    if paramType == "Flow 2":
        title = "Water Flow Rate"
        ylabel = "Flow Rate (L/m)"
        ymax = 50
        ymin = -1
    if paramType == "Flow 3":
        title = "Water Flow Rate"
        ylabel = "Flow Rate (L/m)"
        ymax = 50
        ymin = -1
    if paramType == "Temp":
        title = "Water Temperature"
        ylabel = "Temperature (^C)"
        ymax = 80
        ymin = 0
    if paramType == "EC":
        title = "Electric Conductivity"
        ylabel = "Electric Conductivity (mS/m)"
        ymax = 3000
        ymin = 0
    if paramType == "Turb":
        title = "Turbidity"
        ylabel = "Turbidity (NTU)"
        ymax = 3100
        ymin = -10
    if paramType == "PH":
        title = "pH"
        ylabel = "pH"
        ymax = 14
        ymin = 0
    # haven't put in parameters for battery yet 

    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    xn = range(len(x))  # map your string labels to integers
    # plt.figure(title)
    plt.clf()
    plt.ylim(ymin,ymax)
    plt.plot(xn, y, 'g')
    plt.subplots_adjust(bottom=0.23)
    plt.xlabel('{}'.format(xlabel)) 
    plt.ylabel('{}'.format(ylabel))
    plt.title('{}'.format(title))
    xlabels = list(x)
    for index, item in enumerate(xlabels):
        if (index % 3):
            xlabels[index] = " "
    plt.xticks(xn, xlabels)   # set it to the string values
    plt.xticks(rotation=45)
    plt.xticks(ha='right')
    plt.draw()
    plt.pause(refresh)
    
############################### MAIN CODE ####################################

# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
files = ftpserver.dir()
ftpserver.cwd('/htdocs/')  # change directory to /pub/

# Retrieve file data2.csv
#getfile(ftpserver, 'waterNew.csv')
getfile(ftpserver, 'waterOld.csv')


# Loop to allow user to change data. Note graph becomes unresponsive after refresh pause time has passed
while True:
    print('')
    # input dates
    startDate = input("Start date = ")
    endDate = input("End date = ")
    # input parameter
    readheader('waterOld.csv')
    param = input("Parameter = ")

    readfile('waterOld.csv', param, dictOld)

    # hold values for specific range
    getRange(startDate, endDate, dictOld, dictRange)
    print(sorted(dictRange.items()))

    # plot the graph
    graphLine(dictRange, 8, startDate, endDate, param)

