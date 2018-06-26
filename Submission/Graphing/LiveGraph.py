import ftplib
import csv
from matplotlib import pyplot as plt
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


# Function to read CSV file and create a dictionary with key as the time stamp and measurement as value
def readfile(filename, parameter, dictionary):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            time = row['Time']
            dictionary[time] = float(row['{}'.format(parameter)])
            

# Function to append a data file on the FTP server
def appendfile(ftp, filename):
    ftp.storbinary("APPE " + filename, open(filename, 'rb'))


# Function to upload a file to the FTP server
def uploadfile(ftp, filename):
    ftp.storbinary("STOR " + filename, open(filename, 'rb'))


# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, xlabel, ylabel, title, refresh, ymin, ymax, row, column, index, colour):
    # Extracts dictionary into two tuples
    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    # Extract last 51 values to plot
    x = x[-51:]
    y = y[-51:]
    # Map your string labels to integers
    xn = range(len(x))
    plt.subplot(row, column, index)
    plt.cla()
    plt.ylim(ymin,ymax)
    plt.plot(xn, y, colour)
    plt.subplots_adjust(bottom=0.15)
    plt.xlabel('{}'.format(xlabel)) 
    plt.ylabel('{}'.format(ylabel))
    plt.title('{}'.format(title))
    # Strips out labels to avoid crowding, helping user to better read the labels
    xlabels = list(x)
    for index, item in enumerate(xlabels):
        if (index % 3):
            xlabels[index] = " "
    plt.xticks(xn, xlabels)   # set it to the string values
    plt.xticks(rotation=45)
    plt.xticks(ha='right')
    plt.draw()
    plt.pause(refresh)

################################## MAIN CODE ####################################


while True:
    try:
        # Open ftp connection
        ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')
        ftpserver.cwd('/htdocs/')  # change directory to /htdocs/
        # Initialise dictionaries for data storage
        dictLevel = dict()
        dictTurb = dict()
        dictFlow = dict()
        dictPH = dict()
        dictEC = dict()
        dictTemp = dict()
        # Retrieve file from server
        getfile(ftpserver, 'waterOld.csv')
        print("checking for new data...")
        if "waterNew.csv" in ftpserver.nlst():
            # Sleep so that download doesn't interrupt the data upload from the device
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
                        # make sure that waterOld.csv has the header names even though there's no data
                        with open('waterOld.csv', 'a') as csvfile:
                            print("got content")
                            print(row)
                            w = csv.writer(csvfile)
                            w.writerow(row)

            print("delete file")
            ftpserver.delete('waterNew.csv')
            
                
        plt.figure('Water Measurements')

        # Read in data and plot to sub plots on the same figure
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

        # upload file to server
        uploadfile(ftpserver, 'waterOld.csv')

        ftpserver.close()
    # Break the loop using "ctrl c"
    except KeyboardInterrupt:
        break

