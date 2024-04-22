from ftplib import FTP
from zipfile import ZipFile
import os
import sys
ftp = FTP('wbach.ddns.net', 'GameData', 'OpenGL@GameData&Tools')

remoteDir = './gameData'
localDir = '../'
filenames = {"Tools.zip", "Data.zip"}
minimumPrints = False

argv = len(sys.argv)
if argv > 1:
   tmpFiles = set()
   for i in range(1, argv):
      if (sys.argv[i] == "silent"):
         minimumPrints = True
      else:
         tmpFiles.add(sys.argv[i])
   if len(tmpFiles) > 0:
      filenames = tmpFiles


ftp.cwd(remoteDir)
#ftp.dir()

ftp.sendcmd("TYPE i")

progress = 0

def getFile(filename):
   global progress
   progress = 0
   fileSize = ftp.size(filename)

   file = open(localDir + filename, 'wb')
   if minimumPrints:
      print("Downloading {fname}...".format(fname=filename))

   def fileWrite(data):
      global progress
      file.write(data)
      progress += len(data)
      if not minimumPrints:
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


