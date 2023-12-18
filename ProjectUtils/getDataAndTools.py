from ftplib import FTP
from zipfile import ZipFile
import os
ftp = FTP('wbach.ddns.net', 'GameData', 'OpenGL@GameData&Tools')

remoteDir = './gameData'
localDir = '../'
filenames = {"Tools.zip", "Data.zip"}

ftp.cwd(remoteDir)
ftp.dir()

ftp.sendcmd("TYPE i")

progress = 0

def getFile(filename):
   global progress
   progress = 0
   fileSize = ftp.size(filename)

   file = open(localDir + filename, 'wb')
   def fileWrite(data):
      global progress
      file.write(data)
      progress += len(data)
      print("Downloading {fname} : {p:.2f}%\r".format(fname=filename, p=progress/fileSize * 100), end="")

   ftp.retrbinary(f"RETR {filename}", fileWrite)
   file.close()

for filename in filenames:
   if os.path.exists(localDir + filename):
      print("File exist {} skip.".format(localDir + filename))
      continue

   getFile(filename)
   print("Extracting {}".format(localDir + filename) + "              ")
   with ZipFile(localDir + filename, 'r') as f:
    f.extractall(localDir)

# print("\n")
# for filename in filenames:
#    print("Excracting file : " + filename)
#    with ZipFile(localDir + filename, 'r') as f:
#     f.extractall(localDir)

print("Done")
ftp.quit()


