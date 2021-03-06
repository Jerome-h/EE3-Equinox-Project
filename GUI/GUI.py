import ftplib
import csv
import datetime
from matplotlib import pyplot as plt

Temps = dict()
plt.ion()


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


# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, title, xlabel, ylabel, refresh):
    lists = sorted(dictionary.items())
    print(lists)
    x, y = zip(*lists)
    xn = range(len(x))  # map your string labels to integers
    plt.figure(title)
    plt.clf()
    plt.plot(xn, y, 'r', marker='o')
    plt.xticks(xn, x)   # set it to the string values
    plt.xlabel('{}'.format(xlabel)) 
    plt.ylabel('{}'.format(ylabel))
    plt.title('{}'.format(title))
    plt.draw()
    plt.pause(refresh)


# While loop for constant updating of the data
while True:
    # Open ftp connection
    ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

    # List the files in the current directory
    print("File List:")
    files = ftpserver.dir()
    ftpserver.cwd('/htdocs/')  # change directory to /pub/

    # Retrieve desired files
    getfile(ftpserver, 'data.csv')
    ftpserver.close()

    # Read files into dictionaries to hold values
    readfile('data.csv', 'Temp', Temps)
    print(Temps)
    # Graph data
    graphLine(Temps, 'Temperature', 'Time (s)', 'Temp (^C)', 1)
    #graphLine(Temps, 'Temperature2', 'Time (s)', 'Temp (^C)', 1)
    # plt.show()
