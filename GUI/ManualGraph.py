import ftplib
import csv
from matplotlib import pyplot as plt
import re

# Initialise dictionaries to store data
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


# Function to read CSV file and create a dictionary with key as the time stamp and measurement as value
def readfile(filename, parameter, dictionary):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            time = row['Time']
            dictionary[time] = float(row['{}'.format(parameter)])
            

# Function to read and print the header names of the file, with 'Time' omitted
def readheader(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        print("Enter one of the following parameters:")
        headers = list(next(reader))
        for index, item in enumerate(headers):
            if item == 'Time' or item == '':
                del(headers[index])
        print(headers)


# Function to append a data file on the FTP server
def appendfile(ftp, filename):
    ftp.storbinary("APPE " + filename, open(filename, 'rb'))


# Function to upload file to the FTP server
def uploadfile(ftp, filename):
    ftp.storbinary("STOR " + filename, open(filename, 'rb'))


# Function creates dictionary of data within specified time range
def getRange(startDate, endDate, dictionary, dictionaryRange):
    startFound = False
    endFound = False
    # Key is the time stamp
    for key in sorted(dictionary):
        # Determines when the key is greater than the specified start, sets bool startFound to true
        if startDate <= key:
            startFound = True
        # If the end date is not found in the key and endFound is true, will terminate loop as all entries are found
        if not re.search(endDate, '{}'.format(key)) and endFound == True:
            break
        # If startFound is true the following values are within the specified range, so are added to the new dictionary
        if startFound == True:
            dictionaryRange[key] = dictionary[key]
        # Determines when the key is greater than the specified end, sets bool endFound to true
        if endDate <= key:
            endFound = True


# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, refresh, startDate, endDate, paramType):
    xlabel = 'Date and Time'
    # Defines graph properties dependant on parameter type specified
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

    # Extracts dictionary into two tuples
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
    
############################### MAIN CODE ####################################


# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
files = ftpserver.dir()
ftpserver.cwd('/htdocs/')  # change directory to /htdocs/

# Retrieve file from server
# getfile(ftpserver, 'waterNew.csv')
getfile(ftpserver, 'waterOld.csv')

# Loop to allow user to change plot. Note graph becomes unresponsive after refresh pause time has passed
while True:
    print('')
    # input dates
    print("Enter the date range in the format 'year/month/day hour/minute/second: ")
    startDate = input("Start date = ")
    endDate = input("End date = ")
    # input parameter
    readheader('waterOld.csv')
    param = input("Parameter = ")

    # Read file into dictionary for specified parameter
    readfile('waterOld.csv', param, dictOld)

    # Retrieve values for specific time range
    getRange(startDate, endDate, dictOld, dictRange)
    print(sorted(dictRange.items()))

    # Plot the graph
    graphLine(dictRange, 8, startDate, endDate, param)

