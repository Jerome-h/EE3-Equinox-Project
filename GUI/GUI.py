from matplotlib import pyplot as plt
import ftplib


def getfile(ftp, filename):
    try:
        ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
    except:
        print
        "Error"


def graph(temps, refresh):
    plt.plot(temps, 'r', label='^C', marker='o')
    plt.legend()

    axes = plt.gca()

    plt.draw()
    plt.pause(refresh)
    plt.clf()


# Open ftp connection
ftp = ftplib.FTP('ftp.drivehq.com', 'equinox_eee', 'equinox1234')

# List the files in the current directory
print("File List:")
files = ftp.dir()
print(files)

ftp.cwd('/')  # change directory to /pub/
getfile(ftp, 'data.txt')
ftp.close()
# Print the readme file contents
print("\nData File Output:")
gFile = open("data.txt", "r")
buff = gFile.read()
print(buff)
gFile.close()

