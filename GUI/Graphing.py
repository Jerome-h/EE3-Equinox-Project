import ftplib
import csv
import datetime
from matplotlib import pyplot as plt
import os
import time

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

# Function to send data file to the FTP server
def uploadfile(ftp, filename):
    ftp.storbinary("APPE " + filename, open(filename, 'rb'))

# Function to send file to the FTP server
def uploadfile2(ftp, filename):
    ftp.storbinary("STOR " + filename, open(filename, 'rb'))

# Plots the parameter dictionary as a line graph, also passes label inputs
def graphLine(dictionary, title, xlabel, ylabel, refresh):
    lists = sorted(dictionary.items())
    x, y = zip(*lists)
    xn = range(len(x))  # map your string labels to integers
    plt.figure(title)
    plt.clf()
    plt.ylim(0, 45)
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

# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
files = ftpserver.dir()
ftpserver.cwd('/htdocs/')  # change directory to /pub/

while True:
    try:
        # Retrieve boolean file
        getfile(ftpserver, "bool.txt")
        print("checking if new data is uploaded to the FTP server..")
        time.sleep(0.5)
        f = open("bool.txt","r")
        
        buff = dict()

        # Retrieve file data2.csv
        getfile(ftpserver, 'data2.csv')
    
        # Read file data2.csv into dictionaries to hold values
        readfile('data2.csv', 'Flow Rate', buff)

        # if bool is TRUE, update graph and data.csv in the server
        if f.read() == 'TRUE': 
            f.close()
            # Write the dictionary that is to be appended back to data.csv    
            with open('data.csv','w') as csvfile: 
                w = csv.writer(csvfile)
                w.writerows(sorted(buff.items()))

            # Merge the latest dictionary to the previous dictionary before graphing
            Temps = merge_two_dicts(Temps, buff)

            print("Uploading file now . . .")
            uploadfile(ftpserver, "data.csv") # data.csv will now have the new data
            with open("bool.txt","w") as f:
                f.write("FALSE") 
            f.close()
            uploadfile2(ftpserver, "bool.txt") # set bool to FALSE

        # show data from data.csv
        getfile(ftpserver, 'data.csv')
        f = open('data.csv', 'r')
        print('Data content of data.csv')
        print(f.read())
        f.close()
        # Graph updated data
        graphLine(Temps, 'Flow Rate', 'Date and Time', 'Flow Rate mL/s', 1)

    # break the loop using "ctrl c"
    except KeyboardInterrupt:
        break

