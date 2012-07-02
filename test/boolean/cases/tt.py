types = ["none",
         "s0e0", "s0e1", "s0e2",
         "s1e0", "s1e1", "s1e2",
         "s2e3"]

file = open("test/boolean/cases/tt.cas", "r")

cases = []
header = []
for line in file:
  words = line.strip().split()

  if words[0]!=".":
    header = words[0:3]
  else:
    words[0:3] = header
  segment = (words[6]=="1")

  words_type = [-1]*6
  for i in range(6):
    for j in range(len(types)):
      if words[i]==types[j]:
        words_type[i] = j
        break
    if words_type[i]==-1:
      print "::::::::::::::::::::::::::::::::::::::::::::"
      print words[i]
      print "::::::::::::::::::::::::::::::::::::::::::::"

  types1 = words_type[0:3]
  types2 = words_type[3:6]

  types1.sort()
  types2.sort()

  words_type[0:3] = types1
  words_type[3:6] = types2
  if segment:
    words_type.append(1)
  else:
    words_type.append(0)

  cases.append(words_type)

def cc(ci,cj):
  if ci[0]<cj[0]:
    return -1
  elif ci[0]>cj[0]:
    return 1
  else:
    return cc(ci[1:],cj[1:])

cases.sort(cmp=cc)

finded = []
for i in xrange(len(cases)):
  if i in finded:
    continue

  ci = cases[i]
  ok = False
  printed = False
  for j in xrange(i+1,len(cases)):
    cj = cases[j]
    if ci[0]==cj[3] and ci[1]==cj[4] and ci[2]==cj[5] and cj[0]==ci[3] and cj[1]==ci[4] and cj[2]==ci[5]:
      wi = [types[ci[0]], types[ci[1]], types[ci[2]], str(ci[6])]
      wj = [types[cj[0]], types[cj[1]], types[cj[2]], str(cj[6])]
      li = " ".join(wi)
      lj = " ".join(wj)
      if (ci[6]==1 or cj[6]==1) and (ci[6]==0 or cj[6]==0):
        print "%s\n%s"%(li, lj)
        printed = True
      ok = True
      finded.append(j)

  if ok:
    if printed:
      print ":::::::::::::::::::::::::::::::::::::::::::::::::::"
  elif ci[0]!=ci[3] or ci[1]!=ci[4] or ci[2]!=ci[5]:# caso no autosimetrico sin correspondencia
    wi = [types[ci[0]], types[ci[1]], types[ci[2]], types[ci[3]], types[ci[4]], types[ci[5]], str(ci[6])]
    li = " ".join(wi)
    print "::::::::::::::::::::::::::::::::::::::::::::::::::: %s"%(li)
  else:# caso autosimetrico
    wi = [types[ci[0]], types[ci[1]], types[ci[2]], str(ci[6])]
    li = " ".join(wi)
    if ci[6]==1:
      print li
      print ":::::::::::::::::::::::::::::::::::::::::::::::::::"
