#/**************************************************************************
# * Kevin 2014-02-24                                                       *
# **************************************************************************/

#---------------------------------------------------------------------------
# Constant Variable definition
#---------------------------------------------------------------------------

ifeq ($(COMPILER_OS_CYGWIN),1)
TOPDIR=../../..
endif

include $(TOPDIR)/Tools/toolchain.def

CFLAGS		+= -DTLS_OS_LITEOS=1
#---------------------------------------------------------------------------
# Target definition (User)
#---------------------------------------------------------------------------
GOAL  = $(LIB_DIR)/wmrtos.$(LIBTYPE)

#---------------------------------------------------------------------------
# Source section (User)
#---------------------------------------------------------------------------
ifeq ($(TOOL_GNU),1)
#ASM_SRC				+= ports/portASM_gcc.s
ASM_SRC				+= arch/arm/arm-m/cortex-m3/gcc/los_dispatch_gcc.S
else
ifeq ($(ARCH_M3), 0)
ASM_SRC				+= ports/portASM.s
else
ASM_SRC				+=
endif
endif

ifeq ($(ARCH_M3), 0)
C_SRC				+= ports/port.c
endif
C_SRC				+= ${wildcard kernel/*.c}
C_SRC				+= ${wildcard kernel/base/core/*.c}
C_SRC				+= ${wildcard kernel/base/ipc/*.c}
C_SRC				+= ${wildcard kernel/base/mem/bestfit_little/*.c}
C_SRC				+= ${wildcard kernel/base/mem/membox/*.c}
C_SRC				+= ${wildcard kernel/base/mem/common/*.c}
C_SRC				+= ${wildcard kernel/base/misc/*.c}
C_SRC				+= ${wildcard kernel/base/om/*.c}
C_SRC                           += ${wildcard kernel/extended/tickless/*.c}        
C_SRC                           += ${wildcard arch/arm/arm-m/src/*.c}        
C_SRC                           += wm_osal_liteos.c
        
        
        
        
        
        
        
        


ifeq ($(ARCH_M3), 1)
ifeq ($(TOOL_GNU),1)
#C_SRC				+= ports/port_m3_gcc.c
else
#C_SRC				+= ports/port_m3.c
endif
endif
#---------------------------------------------------------------------------
# Implicit rules
#---------------------------------------------------------------------------
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $< $(INCLUDES)

.s.o:
	$(ASM) $(ASMFLAGS) -c -o $*.o $< $(INCLUDES)

.S.o:
	$(ASM) $(ASMFLAGS) -c -o $*.o $< $(INCLUDES)
#---------------------------------------------------------------------------
# Explicit ruls
#---------------------------------------------------------------------------
OBJ_FILES	= $(C_SRC:%.c=%.o) $(ASM_SRC:%.S=%.o)

all: $(GOAL)

$(GOAL): $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $(OBJ_FILES)

.PHONY: clean
clean:
	$(RM) -f $(GOAL)
	$(RM) -f $(OBJ_FILES:.o=.d) $(OBJ_FILES)
	
