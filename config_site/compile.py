names = ["about", "index", "ota"]
out = "ret.c"

open(out, "w").close()
for name in names:
  f = open(name + ".html", "r")
  fin = f.read()
  fout = ["\"" + x + "\\" + "n" + "\"" for x in fin.replace("\"","\'").splitlines()]
  with open("ret" + ".c", "a") as f:
    f.write("const char* " + name + "Html = ")
    for i in fout:
      f.write("\n"+i)
    f.write(";")
    f.write("\n")