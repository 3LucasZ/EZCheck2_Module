name = "otaRaw"

f = open(name + ".html", "r")
fin = f.read()
fout = ["\"" + x + "\\" + "n" + "\"" for x in fin.replace("\"","\'").splitlines()]
with open(name + ".c", "w+") as f:
  f.write("const char* " + "otaHtml = ")
  for i in fout:
    f.write("\n"+i)
  f.write(";")