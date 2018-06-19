import ftplib
import csv
import datetime
from matplotlib import pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.cm as cm
import matplotlib.colors
import numpy as np
import os
import time
import re

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
        print("Enter one of the following parameters except 'Time':")
        print(next(reader))

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
def graphLine(dictionary, xlabel, ylabel, title, refresh, ymin, ymax, row, column, index, colour):
    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    x = x[-51:] # Extract last 21 values to plot
    y = y[-51:]
    xn = range(len(x))  # map your string labels to integers
    plt.subplot(row, column, index)
    plt.cla()
    plt.ylim(ymin,ymax)
    plt.plot(xn, y, colour)
    plt.subplots_adjust(bottom=0.15)
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

    
# merging 2 dicitionaries
def merge_two_dicts(x, y):
    z = x.copy()   # start with x's keys and values
    z.update(y)    # modifies z with y's keys and values & returns None
    return z

################################## MAIN CODE ####################################


while True:
    try:
        # Open ftp connection
        ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
        #files = ftpserver.dir()
        ftpserver.cwd('/htdocs/')  # change directory to /htdocs/

        dictLevel = dict()
        dictTurb = dict()
        dictFlow = dict()
        dictPH = dict()
        dictEC = dict()
        dictTemp = dict()
        
        getfile(ftpserver, 'waterOld.csv')
        print("checking for new data...")
        if "waterNew.csv" in ftpserver.nlst():
            time.sleep(17)
            print("FILE FOUND")
            getfile(ftpserver, 'waterNew.csv')
            
            # Read file from waterNew.csv and append them to waterOld.csv
            with open('waterNew.csv') as csvfile:
                reader = csv.reader(csvfile)
                for row in reader:
                    if re.search('Time', str(row)):
                        print("found header")
                    else:
                        with open('waterOld.csv', 'a') as csvfile:
                            print("got content")
                            print(row)
                            w = csv.writer(csvfile)
                            w.writerow(row)

            print("delete file")
            ftpserver.delete('waterNew.csv')
            
                
        plt.figure('Water Measurements')
        
        readfile('waterOld.csv', 'Level', dictLevel)
        graphLine(dictLevel, 'Date and Time', 'Level (m)', 'Water Level', 0.1, -0.005, 1, 2, 3, 1, 'b')

        readfile('waterOld.csv', 'Flow 2', dictFlow)
        graphLine(dictFlow, 'Date and Time', 'Flow Rate (L/hr)', 'Flow Rate', 0.1, -1, 300, 2, 3, 2, 'c')

        readfile('waterOld.csv', 'Turb', dictTurb)
        graphLine(dictTurb, 'Date and Time', 'Turbidity (NTU)', 'Turbidity', 0.1, -10, 3100, 2, 3, 3, 'g')

        readfile('waterOld.csv', 'PH', dictPH)
        graphLine(dictPH, 'Date and Time', 'pH', 'pH', 0.1, 0, 14, 2, 3, 4, 'm')

        readfile('waterOld.csv', 'Temp', dictTemp)
        graphLine(dictTemp, 'Date and Time', 'Temperature (^C)', 'Temperature', 0.1, 0, 80, 2, 3, 5, 'r')

        readfile('waterOld.csv', 'EC', dictEC)
        graphLine(dictEC, 'Date and Time', 'EC', 'Electric Conductivity (uS/m)', 0.1, 0, 8000, 2, 3, 6, 'k')

        plt.tight_layout()
        
        #plt.cla()
        uploadfile(ftpserver, 'waterOld.csv')

        ftpserver.close()
    # break the loop using "ctrl c"
    except KeyboardInterrupt:
        break
