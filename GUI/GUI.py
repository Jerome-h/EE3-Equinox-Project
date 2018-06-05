import ftplib
import csv
from matplotlib import pyplot as plt

Temps = dict()

plt.ion()

# Function to get file from the ftp server and save locally
def getfile(ftp, filename):
    try:
        ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
    except:
        print
        "Error"


# Function to read CSV file and create a dictionary with key as time and parameter as value
def readfile(filename, parameter, dictionary):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            time = row['Time']
            dictionary[time] = row['%s' % parameter]


# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, title, xlabel, ylabel, refresh):
    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    plt.figure(title)
    plt.clf()
    plt.plot(x, y, 'r', marker='o')
    plt.xlabel('%s' % xlabel)
    plt.ylabel('%s' % ylabel)
    plt.title('%s' % title)
    plt.draw()
    plt.pause(refresh)





while True:
    # Open ftp connection
    ftpserver = ftplib.FTP('ftp.drivehq.com', 'equinox_eee', 'equinox1234')

    # List the files in the current directory
    print("File List:")
    files = ftpserver.dir()
    print(files)
    ftpserver.cwd('/')  # change directory to /pub/

    # Retrieve desired files
    getfile(ftpserver, 'data.csv')
    ftpserver.close()
    
    # Read files into dictionaries to hold values
    readfile('data.csv', 'Temp', Temps)

    # Graph data
    graphLine(Temps, 'Temperature', 'Time (s)', 'Temp (^C)', 1)
    graphLine(Temps, 'Temperature2', 'Time (s)', 'Temp (^C)', 1)
    # plt.show()


