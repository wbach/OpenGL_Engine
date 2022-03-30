import sys
import os
import shutil

mypath = "."
if  len(sys.argv) > 1:
	mypath = sys.argv[1]
inputFilename=""
if  len(sys.argv) > 2:
	inputFilename = sys.argv[2]
preferedPath=""
if  len(sys.argv) > 3:
	preferedPath = sys.argv[3]

print("Search in : " + os.path.abspath(mypath))

# onlyfiles = [f for f in os.listdir(mypath) if os.path.isfile(os.path.join(mypath, f))]

# for file in onlyfiles:
# 	print(file)

basenames=[]
filenames=[]
mainfile=""

def findFiles(path, prefix = ""):
	global mainfile
	localprefix = prefix + "---"
	for f in os.listdir(path):
		ff = os.path.join(path, f)
		if os.path.isfile(ff):
			if inputFilename:
				if f != inputFilename:
					continue

			if preferedPath and preferedPath in ff:
				print("exist")
				mainfile = ff

			basenames.append(f)
			filenames.append(ff)

			#print(localprefix + "(f): " + ff)
		elif os.path.isdir(ff):
			#print(localprefix + "(d) : " + ff)
			findFiles(ff, localprefix)

findFiles(mypath)

# print("All basenames: ")
# for f in basenames:
# 	print(f)
# print("All filenames: ")

print("All duplicates: ")
toRemoveDir="./duplicates/"
if not os.path.exists(toRemoveDir):
	os.makedirs(toRemoveDir)

if mainfile:
	i = 0
	for f in filenames:
		if f == mainfile:
			print("stay " + f)
			continue
		shutil.move(f, toRemoveDir + str(i) + "_" + os.path.basename(f))
		i = i + 1
		print("to remove :" + f)
else:
	if filenames:
		if preferedPath:
			#shutil.move(filenames[0], 
			print("to move and stay " + filenames[0])

		for f in filenames[1:]:
			print("to remove :" + f)

# for f in filenames:
# 	os.path.dirname(filename)
# 	directory = "./common/";
# 	if not os.path.exists(directory):
#     	os.makedirs(directory)
#  	print(f)