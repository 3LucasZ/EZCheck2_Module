# names = ["about", "index", "ota"]
inputFiles = ["index.html"]
outputFile = "ret.c"
delimeter1 = "[[[[["
delimeter2 = "]]]]]"

open(outputFile, "w").close() #clear the output file
for inputFile in inputFiles: #loop through input files
  params = []
  fileContent = open(inputFile, "r").read()
  formattedFileLines = ["\"" + fileLine + "\\" + "n" + "\"" for fileLine in fileContent.replace("\"","\'").splitlines()]

  middle = []
  for line in formattedFileLines:
    split = line.split(delimeter1)
    if (len(split)==2):
      param = split[1].split(delimeter2)[0]
      if (param not in params):
        params.append(param)
      line = line.replace(delimeter1+param+delimeter2,"\" + "+param +" + \"")
    middle.append(line)
  print(params)

  start = "String getIndex("
  for i in range(len(params)):
    start += "String " + params[i]
    if (i!=len(params)-1):
      start += ", "
  start += ") {"

  with open(outputFile, "a") as f:
    f.write(start + "\n")
    f.write("String ret = ")
    for line in middle: f.write(line + "\n")
    f.write(";")
    f.write("return ret;")
    f.write("}")
  






# with open(outputFile, "a") as f:
#     f.write("const char* " + inputFile + "Html = ")
#     for i in fout:
#       split = i.split(delimeter1)
#       if (len(split)==2):
#         param = split[1].split(delimeter2)[0]
#         i = i.replace(delimeter1+param+delimeter2,"\" + "+param +" + \"")
#       f.write("\n"+i)
#     f.write(";")
#     f.write("\n")




