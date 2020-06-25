from aleatorio import aleatorio
from guloso import guloso
import glob
import os
import sys
import operator
import csv

folder=sys.argv[1]
Type=sys.argv[2]
clients=[sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6]]
ip=sys.argv[7]
simu=sys.argv[8]
delays=[sys.argv[9],sys.argv[10],sys.argv[11],sys.argv[12]]
Servers={}
ServersIP=["1.0.0.1","2.0.0.1","3.0.0.1","4.0.0.1"]
client=[float(sys.argv[13]),int(sys.argv[14]),float(sys.argv[15])]
T=[float(sys.argv[16])/10000, float(sys.argv[17])/20000,float(sys.argv[18])/20000,float(sys.argv[19])/100000]
#Tp=float(sys.argv[13])
#St= int(sys.argv[14])
#Rb= float(sys.argv[15])
gul = guloso(log=True)
al = aleatorio(log=True)

def main():
  os.chdir(folder)
  concatenarServers()
  if (Type=="guloso"):
    selecionado=gul.Politica(Servers,ip,client)
  else:
    selecionado=al.Politica(Servers,ip,client)
  print(selecionado)

def concatenarServers():
  StallValues=[0,0,0,0]
  RebufferValues=[0,0,0,0]
  ThroughputValues=[0,0,0,0]
  files= '*sim{simu}_RebufferLog*'.format(simu=simu)
  RebufferFile = glob.glob(files)
  name = RebufferFile[0]
  file = open(name,"r")
  next(file)
  for line in file:
    fields = line.split(";")
    RebufferValues[0]=float(fields[1])
    RebufferValues[1]=float(fields[3])
    RebufferValues[2]=float(fields[5])
    RebufferValues[3]=float(fields[7])

    files= '*sim{simu}_StallLog*'.format(simu=simu)
    StallFile = glob.glob(files)
    name = StallFile[0]
    file = open(name,"r")
    next(file)
    for line in file:
      fields = line.split(";")
      StallValues[0]=int(float(fields[1]))
      StallValues[1]=int(float(fields[3]))
      StallValues[2]=int(float(fields[5]))
      StallValues[3]=int(float(fields[7]))

    files= '*throughputServer_sim{simu}_*'.format(simu=simu)
    throughputFiles = glob.glob(files)
    throughputFiles.sort()
    j=0
    while(j<len(throughputFiles)):
      name = throughputFiles[j]
      file = open(name,"r")
      next(file)
      for line in file:
        fields = line.split(";")
        ThroughputValues[j]=float(fields[1])
      j+=1

    for i in range(0,len(delays)):
      aux=list(delays[i])
      aux=aux[:-10]
      aux.pop(0)
      delays[i] = ''.join(aux)
      delays[i] = int(delays[i])

    for i in range(0,4):
      ServerIP = ServersIP[i]
      Servers[ServerIP] = [delays[i],ThroughputValues[i],StallValues[i],RebufferValues[i],T[i]]#int(float(clients[i]))]
    return Servers

if __name__=="__main__":
    main()
