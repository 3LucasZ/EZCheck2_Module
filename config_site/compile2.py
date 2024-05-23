# names = ["about", "index", "ota"]
inputFiles = ["index.html"]
outputFile = "ret.c"
delimeter1 = "[[[[["
delimeter2 = "]]]]]"



open(outputFile, "w").close() #clear the output file
for name in inputFiles:
  f = open(name + ".html", "r")
  fin = f.read()
  fout = ["\"" + x + "\\" + "n" + "\"" for x in fin.replace("\"","\'").splitlines()]
  

middle = []
with open(outputFile, "a") as f:
    f.write("const char* " + name + "Html = ")
    for i in fout:
      split = i.split(delimeter1)
      if (len(split)==2):
        inner_str = split[1].split(delimeter2)[0]
        i = i.replace(delimeter1+inner_str+delimeter2,"\" + "+inner_str +" + \"")
      f.write("\n"+i)
    f.write(";")
    f.write("\n")

start = "string getIndex"

open(outputFile, "w").close()
for name in inputFiles:
  f = open(name + ".html", "r")
  fin = f.read()
  fout = ["\"" + x + "\\" + "n" + "\"" for x in fin.replace("\"","\'").splitlines()]
  