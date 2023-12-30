f = open("form.html", "r")
fin = f.read()
fout = ["\"" + x + "\"" for x in fin.replace("\"","\'").splitlines()]
with open("out.c", "w+") as f:
  f.write("const char* serverIndex = ")
  for i in fout:
    f.write("\n"+i)
  f.write(";")