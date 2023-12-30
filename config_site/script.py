name = "ota"

f = open(name + ".html", "r")
fin = f.read()
fout = ["\"" + x + "\"" for x in fin.replace("\"","\'").splitlines()]
with open(name + ".c", "w+") as f:
  f.write("const char* " + name + "Html = ")
  for i in fout:
    f.write("\n"+i)
  f.write(";")