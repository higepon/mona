TARGET   = JHELLO.EXE
JSOURCES = Main.java
JAVAMAIN = Main
JAVADIR  = ../../Devel/libjava

include $(JAVADIR)/Makefile.inc

LDFLAGS += -ljava-imp -lsms_gc-imp -lmonalibc-imp -lmonapi-imp
JFLAGS  += --encoding=UTF-8
CLSPATH += --classpath=$(JAVADIR):.

$(TARGET): $(JSOURCES) $(OBJECTS)
	ld --Map $(TARGET).map -n --image-base=0xa0000000 -e _user_start -o $(TARGET) $(MONADIR)/lib/monapi.o $(OBJECTS) -L$(MONADIR)/lib -ljava-imp -lsms_gc-imp -lmonalibc-imp -lmonapi-imp
	strip $(TARGET)
	bim2bin in:$(TARGET) out:JHELLO.EX5 -osacmp -tek5 BS:0 eprm:z0
	install -p JHELLO.EX5 $(MONADIR)/bin/APPS

clean:
	rm -f $(TARGET) JHELLO.EX5 $(JCLASSES) *.map *.stackdump *.o
