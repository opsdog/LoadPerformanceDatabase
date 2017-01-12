##  makefile

CC= gcc

CFLAGS=

##
##  these need to match where the TokenParse stuff is
##

INCLUDEDIRS= -I/home/douggreenwald/include 
LIBDIRS= -L/home/douggreenwald/lib 

##
##  mysql specific options
##

MYSQLINC= -I/usr/include/mysql 
MYSQLLIBS= -L/usr/lib64/mysql -lmysqlclient # -lz -lm

SANPARSE= LoadHTC-RResp LoadHTC-RIOPs LoadHTC-RTRate \
	LoadHTC-WResp LoadHTC-WIOPs LoadHTC-WTRate \
	LoadHTC01-CSV

Targets= FromEpoch ToEpoch LoadVXstat LoadIOstat LoadIOstat-CSV LoadVXstat-CSV \
	LoadIOstat-CSV-TC LoadIOerrs-CSV-TC LoadVMEM-CSV-TC LoadMPstat-CSV-TC \
	$(SANPARSE) HEXtoDEC-SA LoadProcMem-CSV-TC LoadProcCpu-CSV-TC \
	MDay-ColorMap LoadLinuxIO-CSV-TC LoadNetstat-i-CSV-TC LoadVMstatS-CSV-TC \
	LoadVMstatP-CSV-TC CreateIOCalc CreateVXCalcD CreateVXCalcV \
	LoadNetstat-Lin-CSV-TC LoadIOerrs-Lin-CSV-TC LoadVMstat-Lin-CSV-TC \
	CreateLinuxIOCalc LoadNICstat-CSV-TC CreateIOCalc-ACFS CreateIOCalc-NFS \
	LoadLinuxMP-CSV-TC


.c.o:
	$(CC) -c ${CFLAGS} ${COPTS} ${INCLUDEDIRS} ${MYSQLINC} ${DEBUGFLAGS} $<



all: ${Targets}


MDay-ColorMap: MDay-ColorMap.o
	$(CC) -o MDay-ColorMap MDay-ColorMap.o
	chmod 0755 MDay-ColorMap && echo

FromEpoch: FromEpoch.o
	$(CC) -o FromEpoch FromEpoch.o
	chmod 0755 FromEpoch
	cp -p FromEpoch ~/bin/ && echo

ToEpoch: ToEpoch.o
	$(CC) -o ToEpoch ToEpoch.o
	chmod 0755 ToEpoch
	cp -p ToEpoch ~/bin/ && echo

HEXtoDEC-SA: HEXtoDEC-SA.o
	$(CC) -o HEXtoDEC-SA HEXtoDEC-SA.o ${LINKOPTS}
	chmod 0755 HEXtoDEC-SA
	cp -p HEXtoDEC-SA ~/bin/ && echo




LoadProcMem-CSV-TC: LoadProcMem-CSV-TC.o
	$(CC) -o LoadProcMem-CSV-TC LoadProcMem-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${MYSQLINC} ${LINKOPTS} ${MYSQLLIBS}
	chmod 0755 LoadProcMem-CSV-TC && echo

LoadProcCpu-CSV-TC: LoadProcCpu-CSV-TC.o
	$(CC) -o LoadProcCpu-CSV-TC LoadProcCpu-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${MYSQLINC} ${LINKOPTS} ${MYSQLLIBS}
	chmod 0755 LoadProcCpu-CSV-TC && echo

LoadVXstat: LoadVXstat.o
	$(CC) -o LoadVXstat LoadVXstat.o /home/douggreenwald/lib/TokenParse.o ${MYSQLINC} ${LINKOPTS} ${MYSQLLIBS}
	chmod 0755 LoadVXstat && echo

LoadNICstat-CSV-TC: LoadNICstat-CSV-TC.o
	$(CC) -o LoadNICstat-CSV-TC LoadNICstat-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${MYSQLINC} ${LINKOPTS} ${MYSQLLIBS}
	chmod 0755 LoadNICstat-CSV-TC && echo

LoadIOstat: LoadIOstat.o
	$(CC) -o LoadIOstat LoadIOstat.o /home/douggreenwald/lib/TokenParse.o ${MYSQLINC} ${LINKOPTS} ${MYSQLLIBS}
	chmod 0755 LoadIOstat && echo

LoadIOstat-CSV: LoadIOstat-CSV.o
	$(CC) -o LoadIOstat-CSV LoadIOstat-CSV.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadIOstat-CSV && echo

LoadIOstat-CSV-TC: LoadIOstat-CSV-TC.o
	$(CC) -o LoadIOstat-CSV-TC LoadIOstat-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadIOstat-CSV-TC && echo

LoadIOerrs-CSV-TC: LoadIOerrs-CSV-TC.o
	$(CC) -o LoadIOerrs-CSV-TC LoadIOerrs-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadIOerrs-CSV-TC && echo

LoadIOerrs-Lin-CSV-TC: LoadIOerrs-Lin-CSV-TC.o
	$(CC) -o LoadIOerrs-Lin-CSV-TC LoadIOerrs-Lin-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadIOerrs-Lin-CSV-TC && echo

LoadLinuxIO-CSV-TC: LoadLinuxIO-CSV-TC.o
	$(CC) -o LoadLinuxIO-CSV-TC LoadLinuxIO-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadLinuxIO-CSV-TC && echo

LoadVMEM-CSV-TC: LoadVMEM-CSV-TC.o
	$(CC) -o LoadVMEM-CSV-TC LoadVMEM-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadVMEM-CSV-TC && echo

LoadMPstat-CSV-TC: LoadMPstat-CSV-TC.o
	$(CC) -o LoadMPstat-CSV-TC LoadMPstat-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadMPstat-CSV-TC && echo

LoadLinuxMP-CSV-TC: LoadLinuxMP-CSV-TC.o
	$(CC) -o LoadLinuxMP-CSV-TC LoadLinuxMP-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadLinuxMP-CSV-TC && echo

LoadVXstat-CSV: LoadVXstat-CSV.o
	$(CC) -o LoadVXstat-CSV LoadVXstat-CSV.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadVXstat-CSV && echo

LoadNetstat-i-CSV-TC: LoadNetstat-i-CSV-TC.o
	$(CC) -o LoadNetstat-i-CSV-TC LoadNetstat-i-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadNetstat-i-CSV-TC && echo

LoadNetstat-Lin-CSV-TC: LoadNetstat-Lin-CSV-TC.o
	$(CC) -o LoadNetstat-Lin-CSV-TC LoadNetstat-Lin-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadNetstat-Lin-CSV-TC && echo

LoadVMstatS-CSV-TC: LoadVMstatS-CSV-TC.o
	$(CC) -o LoadVMstatS-CSV-TC LoadVMstatS-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadVMstatS-CSV-TC && echo

LoadVMstatP-CSV-TC: LoadVMstatP-CSV-TC.o
	$(CC) -o LoadVMstatP-CSV-TC LoadVMstatP-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadVMstatP-CSV-TC && echo

LoadVMstat-Lin-CSV-TC: LoadVMstat-Lin-CSV-TC.o
	$(CC) -o LoadVMstat-Lin-CSV-TC LoadVMstat-Lin-CSV-TC.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadVMstat-Lin-CSV-TC && echo


CreateIOCalc: CreateIOCalc.o
	$(CC) -o CreateIOCalc CreateIOCalc.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateIOCalc && echo

CreateIOCalc-ACFS: CreateIOCalc-ACFS.o
	$(CC) -o CreateIOCalc-ACFS CreateIOCalc-ACFS.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateIOCalc-ACFS && echo

CreateIOCalc-NFS: CreateIOCalc-NFS.o
	$(CC) -o CreateIOCalc-NFS CreateIOCalc-NFS.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateIOCalc-NFS && echo

CreateLinuxIOCalc: CreateLinuxIOCalc.o
	$(CC) -o CreateLinuxIOCalc CreateLinuxIOCalc.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateLinuxIOCalc && echo

CreateVXCalcD: CreateVXCalcD.o
	$(CC) -o CreateVXCalcD CreateVXCalcD.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateVXCalcD && echo

CreateVXCalcV: CreateVXCalcV.o
	$(CC) -o CreateVXCalcV CreateVXCalcV.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS} ${MYSQLINC} ${MYSQLLIBS}
	chmod 0755 CreateVXCalcV && echo


LoadHTC-RResp: LoadHTC-RResp.o
	$(CC) -o LoadHTC-RResp LoadHTC-RResp.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-RResp && echo

LoadHTC-RIOPs: LoadHTC-RIOPs.o
	$(CC) -o LoadHTC-RIOPs LoadHTC-RIOPs.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-RIOPs && echo

LoadHTC-RTRate: LoadHTC-RTRate.o
	$(CC) -o LoadHTC-RTRate LoadHTC-RTRate.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-RTRate && echo

LoadHTC-WResp: LoadHTC-WResp.o
	$(CC) -o LoadHTC-WResp LoadHTC-WResp.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-WResp && echo

LoadHTC-WIOPs: LoadHTC-WIOPs.o
	$(CC) -o LoadHTC-WIOPs LoadHTC-WIOPs.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-WIOPs && echo

LoadHTC-WTRate: LoadHTC-WTRate.o
	$(CC) -o LoadHTC-WTRate LoadHTC-WTRate.o DECtoHEX.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC-WTRate && echo

LoadHTC01-CSV: LoadHTC01-CSV.o
	$(CC) -o LoadHTC01-CSV LoadHTC01-CSV.o /home/douggreenwald/lib/TokenParse.o ${LINKOPTS}
	chmod 0755 LoadHTC01-CSV && echo






clean:
	/bin/rm -f *.o *~ \#* core
	/bin/rm -f ${Targets}
	/bin/rm -f edtmp* sadtmp* gc_* tmp_*

OBJClean:
	/bin/rm -f *.o core

bundle:
	tar cf Bundle.tar *.csv
	gzip -f Bundle.tar && echo



##
##  depend relationships
##

FromEpoch.o: FromEpoch.c
ToEpoch.o: ToEpoch.c
HEXtoDEC-SA.o: HEXtoDEC.c
MDay-ColorMap.o: MDay-ColorMap.c

LoadVXstat.o: LoadVXstat.c /home/douggreenwald/lib/TokenParse.o
LoadIOstat.o: LoadIOstat.c /home/douggreenwald/lib/TokenParse.o
LoadNICstat-CSV-TC.o: LoadNICstat-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadIOstat-CSV.o: LoadIOstat-CSV.c /home/douggreenwald/lib/TokenParse.o
LoadIOstat-CSV-TC.o: LoadIOstat-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadIOerrs-CSV-TC.o: LoadIOerrs-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadIOerrs-Lin-CSV-TC.o: LoadIOerrs-Lin-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadLinuxIO-CSV-TC.o: LoadLinuxIO-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadVMEM-CSV-TC.o: LoadVMEM-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadMPstat-CSV-TC.o: LoadMPstat-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadLinuxMP-CSV-TC.o: LoadLinuxMP-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadVXstat-CSV.o: LoadVXstat-CSV.c /home/douggreenwald/lib/TokenParse.o
LoadProcMem-CSV-TC.o: LoadProcMem-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadProcCpu-CSV-TC.o: LoadProcCpu-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadNetstat-i-CSV-TC.o: LoadNetstat-i-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadNetstat-Lin-CSV-TC.o: LoadNetstat-Lin-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadVMstatS-CSV-TC.o: LoadVMstatS-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadVMstatP-CSV-TC.o: LoadVMstatP-CSV-TC.c /home/douggreenwald/lib/TokenParse.o
LoadVMstat-Lin-CSV-TC.o: LoadVMstat-Lin-CSV-TC.c /home/douggreenwald/lib/TokenParse.o

CreateIOCalc.o: CreateIOCalc.c /home/douggreenwald/lib/TokenParse.o
CreateIOCalc-ACFS.o: CreateIOCalc-ACFS.c /home/douggreenwald/lib/TokenParse.o
CreateIOCalc-NFS.o: CreateIOCalc-NFS.c /home/douggreenwald/lib/TokenParse.o
CreateLinuxIOCalc.o: CreateLinuxIOCalc.c /home/douggreenwald/lib/TokenParse.o
CreateVXCalcD.o: CreateVXCalcD.c /home/douggreenwald/lib/TokenParse.o
CreateVXCalcV.o: CreateVXCalcV.c /home/douggreenwald/lib/TokenParse.o

LoadHTC-RResp.o: LoadHTC-RResp.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC-RIOPs.o: LoadHTC-RIOPs.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC-RTRate.o: LoadHTC-RTRate.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC-WResp.o: LoadHTC-WResp.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC-WIOPs.o: LoadHTC-WIOPs.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC-WTRate.o: LoadHTC-WTRate.c DECtoHEX.o /home/douggreenwald/lib/TokenParse.o
LoadHTC01-CSV.o: LoadHTC01-CSV.c /home/douggreenwald/lib/TokenParse.o

