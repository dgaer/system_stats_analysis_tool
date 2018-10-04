# Include file used for all makefiles

include ../3plibs/datareel/env/glibdeps.mak

PROJECT_DEP = $(APP_PATH)$(PATHSEP)m_thread.h \
../include/m_log.h \
../include/stat_collect.h \
../include/cpu_stats.h \
../include/io_stats.h \
../include/load_average_stats.h \
../include/memory_stats.h \
../include/network_stats.h \
../include/proc_stats.h \
../include/swap_stats.h 

M_THREAD_DEP = $(APP_PATH)$(PATHSEP)m_thread.h ../include/m_log.h

M_LOG_DEP = ../include/m_log.h

CPU_STATS_DEP = ../include/cpu_stats.h

IO_STATS_DEP = ../include/io_stats.h

LOAD_AVERAGE_STATS_DEP = ../include/load_average_stats.h

MEMORY_STATS_DEP = ../include/memory_stats.h

NETWORK_STATS_DEP = ../include/network_stats.h

PROC_STATS_DEP = ../include/proc_stats.h

SWAP_STATS_DEP = ../include/swap_stats.h

STAT_COLLECT_DEP = ../include/stat_collect.h \
../include/cpu_stats.h \
../include/io_stats.h \
../include/load_average_stats.h \
../include/memory_stats.h \
../include/network_stats.h \
../include/proc_stats.h \
../include/swap_stats.h 

# ===============================================================

# Compile the files and build the executable
# ===============================================================
all:	$(PROJECT)$(EXE_EXT)

include ../3plibs/datareel/env/glibobjs.mak

m_thread$(OBJ_EXT):	$(APP_PATH)$(PATHSEP)m_thread.cpp $(M_THREAD_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(APP_PATH)$(PATHSEP)m_thread.cpp

m_log$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)m_log.cpp $(M_LOG_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)m_log.cpp

cpu_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)cpu_stats.cpp $(CPU_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)cpu_stats.cpp

io_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)io_stats.cpp $(IO_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)io_stats.cpp

load_average_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)load_average_stats.cpp $(LOAD_AVERAGE_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)load_average_stats.cpp

memory_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)memory_stats.cpp $(MEMORY_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)memory_stats.cpp

network_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)network_stats.cpp $(NETWORK_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)network_stats.cpp

proc_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)proc_stats.cpp $(PROC_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)proc_stats.cpp

stat_collect$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)stat_collect.cpp $(STAT_COLLECT_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)stat_collect.cpp

swap_stats$(OBJ_EXT):	$(ADD_SRC_PATH)$(PATHSEP)swap_stats.cpp $(SWAP_STATS_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(ADD_SRC_PATH)$(PATHSEP)swap_stats.cpp

$(PROJECT)$(OBJ_EXT):	$(APP_PATH)$(PATHSEP)$(PROJECT).cpp $(PROJECT_DEP)
	$(CXX) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(APP_PATH)$(PATHSEP)$(PROJECT).cpp

DATAREEL_OBJECTS = gxssl$(OBJ_EXT) \
asprint$(OBJ_EXT) \
bstreei$(OBJ_EXT) \
btcache$(OBJ_EXT) \
btnode$(OBJ_EXT) \
btstack$(OBJ_EXT) \
cdate$(OBJ_EXT) \
dbasekey$(OBJ_EXT) \
dbfcache$(OBJ_EXT) \
dbugmgr$(OBJ_EXT) \
devcache$(OBJ_EXT) \
dfileb$(OBJ_EXT) \
ehandler$(OBJ_EXT) \
fstring$(OBJ_EXT) \
futils$(OBJ_EXT) \
gpersist$(OBJ_EXT) \
gthreadt$(OBJ_EXT) \
gxbtree$(OBJ_EXT) \
gxcond$(OBJ_EXT) \
gxconfig$(OBJ_EXT) \
gxcrc32$(OBJ_EXT) \
gxdatagm$(OBJ_EXT) \
gxdbase$(OBJ_EXT) \
gxderror$(OBJ_EXT) \
gxdfp64$(OBJ_EXT) \
gxdfptr$(OBJ_EXT) \
gxdlcode$(OBJ_EXT) \
gxdstats$(OBJ_EXT) \
gxfloat$(OBJ_EXT) \
gxint16$(OBJ_EXT) \
gxint32$(OBJ_EXT) \
gxint64$(OBJ_EXT) \
gxip32$(OBJ_EXT) \
gxlistb$(OBJ_EXT) \
gxmac48$(OBJ_EXT) \
gxmutex$(OBJ_EXT) \
gxrdbdef$(OBJ_EXT) \
gxrdbhdr$(OBJ_EXT) \
gxrdbms$(OBJ_EXT) \
gxrdbsql$(OBJ_EXT) \
gxscomm$(OBJ_EXT) \
gxsema$(OBJ_EXT) \
gxsftp$(OBJ_EXT) \
gxshtml$(OBJ_EXT) \
gxsxml$(OBJ_EXT) \
gxsrss$(OBJ_EXT) \
gxshttp$(OBJ_EXT) \
gxshttpc$(OBJ_EXT) \
gxsmtp$(OBJ_EXT) \
gxsocket$(OBJ_EXT) \
gxsping$(OBJ_EXT) \
gxspop3$(OBJ_EXT) \
gxstream$(OBJ_EXT) \
gxsurl$(OBJ_EXT) \
gxsutils$(OBJ_EXT) \
gxs_b64$(OBJ_EXT) \
gxtelnet$(OBJ_EXT) \
gxthread$(OBJ_EXT) \
gxuint16$(OBJ_EXT) \
gxuint32$(OBJ_EXT) \
gxuint64$(OBJ_EXT) \
htmldrv$(OBJ_EXT) \
logfile$(OBJ_EXT) \
memblock$(OBJ_EXT) \
membuf$(OBJ_EXT) \
ostrbase$(OBJ_EXT) \
pod$(OBJ_EXT) \
pscript$(OBJ_EXT) \
scomserv$(OBJ_EXT) \
stdafx$(OBJ_EXT) \
strutil$(OBJ_EXT) \
systime$(OBJ_EXT) \
terminal$(OBJ_EXT) \
thelpers$(OBJ_EXT) \
thrapiw$(OBJ_EXT) \
thrpool$(OBJ_EXT) \
ustring$(OBJ_EXT) \
wserror$(OBJ_EXT)

# Make the executable
OBJECTS = $(PROJECT)$(OBJ_EXT) \
m_thread$(OBJ_EXT) \
m_log$(OBJ_EXT) \
cpu_stats$(OBJ_EXT) \
io_stats$(OBJ_EXT) \
load_average_stats$(OBJ_EXT) \
memory_stats$(OBJ_EXT) \
network_stats$(OBJ_EXT) \
proc_stats$(OBJ_EXT) \
swap_stats$(OBJ_EXT) \
stat_collect$(OBJ_EXT) \
$(DATAREEL_OBJECTS)

# ===============================================================
