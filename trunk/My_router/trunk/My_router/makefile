################################################################################
# Automatically-generated file. Do not edit!
################################################################################

RM := rm -rf

CFLAGS := -O2 -g -Wall -fmessage-length=0
SRCS := stdafx.cpp Utils.cpp RouterSocket.cpp InputHandler.cpp RoutingTable.cpp My_router.cpp MyRouter.cpp
OBJS := stdafx.o Utils.o RouterSocket.o InputHandler.o RoutingTable.o My_router.o MyRouter.o
CC := g++

# To get things out of the revision control system
$(SRCS):
	$(GET) $@
	-@echo 'making'
	$(CC) $(CFLAGS) -c $*.c

# To make an executable

My_router: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)


# All Target
all: My_router

# Other Targets
clean:
	-$(RM) $(OBJS) My_router
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

