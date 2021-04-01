#!/usr/bin/env python3
############################################
# OPC-UA client for the RD53A-OPC-server
# Kaan.Oyulmaz@cern.ch
# Carlos.Solans@cern.ch
# March 2021
############################################
# To install opcua use:
# python -m pip install opcua --user
############################################

from opcua import Client
from opcua import ua

class rd53a_opc_client:
    def __init__(self, u):
        self.client = Client(u)
        self.client.connect()
        pass
    def Load(self):
        nn = self.client.get_objects_node().get_children()[2:]
        self.nodes={}
        for n in nn:
            name=n.get_display_name().Text
            self.nodes[name]={}
            for n2 in n.get_children():
                self.nodes[name][n2.get_display_name().Text]=n2
                pass
            pass
        pass
    def Close(self):
        self.client.disconnect()
        pass
    def PrintServerInfo(self):
        self.PrintNode(self.client.get_root_node())
        pass
    def PrintServerContents(self):
        self.PrintNode(self.client.get_objects_node())
        pass
    def SetVerbose(self, v):    
        self.verbose = v
        pass
    def PrintNode(self,n,lvl=0):
        print("%sNode: %s"% (" "*lvl,n.get_display_name().Text))
        if len(n.get_children())>0:
            for nn in n.get_children():
                self.PrintNode(nn,lvl+1)
                pass
            pass
        pass
    def GetNode(self, n):
        return self.nodes[n]
    pass

if __name__=="__main__":
    
    import os
    import sys
    import signal
    import argparse
    import time
    import datetime

    constr="opc.tcp://pcatlidros02:4841"
    
    parser=argparse.ArgumentParser()
    parser.add_argument('-c','--constr',help="connection string: %s" % constr, default=constr)
    parser.add_argument('-v','--verbose',help="enable verbose mode", action="store_true")
    parser.add_argument('-n','--node',help="node", required=True)
    args=parser.parse_args()

    print("Connect to %s" % args.constr)
    client = rd53a_opc_client(args.constr)
    client.SetVerbose(args.verbose)
    if args.verbose==True:
        client.PrintServerContents()
        pass

    client.Load()
    
    cont = True
    def signal_handler(signal, frame):
        print('You pressed ctrl+C')
        global cont
        cont = False
        return

    signal.signal(signal.SIGINT, signal_handler)

    tstart = int(time.time())
    while True:
        tcurr = int(time.time())
        if cont==False: break
        ss ="Time: %i, " %tcurr
        ss+="ElapsedTime: %4i, " % (tcurr-tstart)
        nn=client.GetNode(args.node)
        for k in nn:
            v = "WaitingForInitialData"
            try: v=nn[k].get_value();
            except: pass;
            ss+="%s: %s, " % (k,v)
            pass
        print(ss)
        time.sleep(1)
        pass
    print("Closing connection")
    client.Close()
    
    print("Have a nice day")
    pass
