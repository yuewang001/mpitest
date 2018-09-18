#
# windriver parallel computing platform.
#
LDFLAGS = -melf_x86_64 -pthread  -m64 \
                  -static_mpi

ALL = emt1

MPICC = mpiicc

THREAD_SAFE=-parallel

WRAPILIB=-lwrapi

all : $(ALL)

objects = emt.o $(WRAPILIB)

emt1: $(objects)
	$(MPICC)   -o $@ $^ $(WITH_DEBUG) -static_mpi $(THREAD_SAFE) $(LDFLAGS)
   
emt.o: emt.c
	$(MPICC) -D_LINUX $(DPDK_FLAG) -fPIC $(WITH_DEBUG) -c $^

clean:
	@rm -f $(ALL)  *.o *.map log 

