TARGET		:= BicycleShed
SOURCES		:= BicycleShed.c			\
		   Display.c Display.h			\
		   GPIOs.c GPIOs.h			\
		   readDatabase.c readDatabase.h	\
		   ServerCom.c ServerCom.h		\
		   Victron.c Victron.h			\
		   Weather.c Weather.h			\
		   ScaleTables.c ScaleTables.h

#
# Derived files
#
CSOURCES	:= $(filter %.c,$(SOURCES))
OBJFILES	:= $(CSOURCES:%.c=%.o)
DEPFILES	:= $(CSOURCES:%.c=%.d)

CLEANABLE_FILES	:= $(TARGET) $(OBJFILES) $(DEPFILES)

#
# Programs and flags
#
CC		:= gcc
CPPFLAGS	:=
CFLAGS		:= `pkg-config --cflags libcurl`	\
		   `pkg-config --cflags libmodbus`	\
		   `mysql_config --cflags`
DEPFLAGS	:= -MM
LDFLAGS		:= `pkg-config --libs libcurl`	\
		   `pkg-config --libs libmodbus`\
		   `mysql_config --libs`	\
		   -lpthread -lwiringPi

#
# Rules
#
all:		$(TARGET)

$(TARGET):	$(OBJFILES)
		$(CC) -o $@ $^ $(LDFLAGS)

%.o:		%.c
		$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.d:		%.c
		$(CC) $(DEPFLAGS) $(CPPFLAGS) $< | sed 's/\($*\).o[ :]*/\1.o $@:/g' > $@

clean:
		-$(RM) $(CLEANABLE_FILES)

ifeq ($(filter clean,$(MAKECMDGOALS)),)
-include $(DEPFILES)
endif

.PHONY:		all clean
