import ftplib
import csv
from matplotlib import pyplot as plt

Temps = []

def getfile(ftp, filename):
    try:
        ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
    except:
        print
        "Error"


def readfile(filename, parameter, array):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            array.append(row['%s' % parameter])


def graph(temps, refresh):
    plt.plot(temps, 'r', label='^C', marker='o')
    plt.legend()

    axes = plt.gca()

    plt.draw()
    # plt.pause(refresh)
    # plt.clf()


# Open ftp connection
ftp = ftplib.FTP('ftp.drivehq.com', 'equinox_eee', 'equinox1234')

# List the files in the current directory
print("File List:")
files = ftp.dir()
print(files)

ftp.cwd('/')  # change directory to /pub/
getfile(ftp, 'data.csv')
ftp.close()
# Print the readme file contents
print("\nData File Output:")

readfile('data.csv', 'Temp', Temps)
graph(Temps, 1)
plt.show()

