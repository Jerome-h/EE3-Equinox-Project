import ftplib
import csv
import datetime
from matplotlib import pyplot as plt
import os
import time
import re

dictOld = dict()
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
def graphLine(dictionary, xlabel, ylabel, title, refresh, ymax):
    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    xn = range(len(x))  # map your string labels to integers
    plt.figure(title)
    plt.clf()
    plt.ylim(0,ymax)
    plt.plot(xn, y, 'g', marker='o')
    plt.xticks(xn, x)   # set it to the string values
    plt.xticks(rotation=45)
    plt.xticks(ha='right')
    plt.subplots_adjust(bottom=0.23)
    plt.xlabel('{}'.format(xlabel)) 
    plt.ylabel('{}'.format(ylabel))
    plt.title('{}'.format(title))
    plt.draw()
    plt.pause(refresh)
    
# merging 2 dicitionaries
def merge_two_dicts(x, y):
    z = x.copy()   # start with x's keys and values
    z.update(y)    # modifies z with y's keys and values & returns None
    return z

################################## MAIN CODE ####################################

# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
files = ftpserver.dir()
ftpserver.cwd('/htdocs/')  # change directory to /pub/

while True:
    try:
        dictOld = dict()
        getfile(ftpserver, 'waterOld.csv')

        if 'waterNew.csv' in ftpserver.nlst():
            print("FILE FOUND")

            getfile(ftpserver, 'waterNew.csv')
            ftpserver.delete('waterNew.csv')

            # Read file from waterNew.csv and append them to waterOld.csv
            with open('waterNew.csv') as csvfile:
                reader = csv.reader(csvfile)
                for row in reader:
                    if re.search('Time', str(row)):
                        print("found header")
                    else:
                        with open('waterOld.csv', 'a') as csvfile:
                            print(row)
                            w = csv.writer(csvfile)
                            w.writerow(row)
                                            
        readfile('waterOld.csv', 'Level', dictOld)
        print(sorted(dictOld))
        graphLine(dictOld, 'Date and Time', 'Level', 'Water Level', 1, 0.4)
        uploadfile(ftpserver, 'waterOld.csv')

        
    # break the loop using "ctrl c"
    except KeyboardInterrupt:
        break

