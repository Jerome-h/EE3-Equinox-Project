import ftplib
import csv
import datetime
from matplotlib import pyplot as plt
import os
import time

dictOld = dict()
dictNew = dict()

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

# merging 2 dicitionaries
def merge_two_dicts(x, y):
    z = x.copy()   # start with x's keys and values
    z.update(y)    # modifies z with y's keys and values & returns None
    return z

############################### MAIN CODE ####################################

# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
files = ftpserver.dir()
ftpserver.cwd('/htdocs/')  # change directory to /pub/

# Retrieve file data2.csv
getfile(ftpserver, 'dataNew.csv')
getfile(ftpserver, 'dataOld.csv')

# Read file data2.csv into dictionaries to hold values
readfile('dataOld.csv', 'Flow Rate', dictOld)
readfile('dataNew.csv', 'Flow Rate', dictNew)

# if bool is TRUE, update graph and data.csv in the server
# Write the dictionary that is to be appended back to data.csv
with open('dataOld.csv','a') as csvfile: 
    w = csv.writer(csvfile)
    w.writerows(sorted(dictNew.items()))


dictOld = dict()
#print(sorted(dictOld.items()))
readfile('dataOld.csv', 'Flow Rate', dictOld)
print("Contents in dataOld: ")
print(sorted(dictOld.items()))

