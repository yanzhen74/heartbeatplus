#g++ yqdsurvey.cpp -DNODEBUG -lnanomsg -o yqdsurvey
NAME = hbclient
EXENAME = heartbeat

INC3 = ../include
LINKDIRS = -L../lib -L/usr/local/lib
LINKLIBS = -lhbclient -lnanomsg
INCLUDES = -I$(INC3)
CFLAGS = -ldl $(INCLUDES) $(LINKDIRS) $(LINKLIBS)

SRCS = client.cpp
OBJS = $(filter %.o, $(SRCS:.cpp=.o))
LIB = lib$(NAME).a

Q ?= @

all: $(EXENAME)

$(EXENAME): $(LIB)
	g++ hearbeat.cpp -DNODEBUG CFLAGS -o $(EXENAME)

$(LIB): $(OBJS)
	@echo '===> Linking $(LIB)'
	$(Q)ar cr $(LIB) $(OBJS)
	$(Q)ranlib $(LIB)
	cp $(LIB) ../lib/$(LIB)

$(OBJS): %.o: %.cpp
	g++ -c $(INCLUDES) $< -o $@

clean:
	-rm -Rf $(LIB) $(OBJS)