import ftplib
import csv
Temps = dict()
# Function to get file from the ftp server and save locally
def getFile(ftp, filename):
    try:
        ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
    except:
        print
        "Error"

def uploadFile(ftp, filename):
    ftp.storbinary("APPE " + filename, open(filename, "rb"), 1024)

# Function to read CSV file and create a dictionary with key as time and parameter as value
def readFile(filename, parameter, dictionary):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            time = int(row['Time'])
            dictionary[time] = int(row['%s' % parameter])

# Open ftp connection
ftpserver = ftplib.FTP('ftp.byethost12.com', 'b12_22196264', 'equinox1234')

# List the files in the current directory
print("File List:")
files = ftpserver.dir()
print(files)
ftpserver.cwd('/htdocs/')  # change directory to /pub/

# Retrieve desired files
getFile(ftpserver, 'data2.csv')

# Read files into dictionaries to hold values
readFile('data2.csv', 'Temp', Temps) # append data2.csv to data.csv
with open('data.csv','w') as f: # data.csv will be the large file
    w = csv.writer(f)
    w.writerows(Temps.items())
    
# Retrieve desired files
uploadFile(ftpserver, 'data.csv') # data.csv will now have the new data
ftpserver.close()

