CC=g++ -g
INCL=-I../include/
OBJ_TGT=../objects/
INCL_TGT=../include/

all: $(OBJ_TGT)ocMail.o $(OBJ_TGT)oc_Mail.o mailTest $(INCL_TGT)ocMail.h $(INCL_TGT)oc_Mail.h

mailTest: mailTest.o $(OBJ_TGT)oc_Mail.o
	$(CC) -o mailTest mailTest.o $(OBJ_TGT)oc_Mail.o  -luuid
  
mailTest.o: mailTest.cpp ocMail.h oc_Mail.h
	$(CC) -c  mailTest.cpp $(INCL)
  
$(OBJ_TGT)ocMail.o: ocMail.cpp ocMail.h
	$(CC) -c  $(INCL) ocMail.cpp -o $(OBJ_TGT)ocMail.o
 
$(INCL_TGT)ocMail.h: ocMail.h
	cp ocMail.h  $(INCL_TGT)

$(OBJ_TGT)oc_Mail.o: oc_Mail.cpp oc_Mail.h  $(INCL_TGT)oc_SMTP.h
	$(CC) -c  $(INCL) oc_Mail.cpp -o $(OBJ_TGT)oc_Mail.o

$(INCL_TGT)oc_Mail.h: oc_Mail.h
	cp oc_Mail.h  $(INCL_TGT)
 
clean:
	rm *.o
	rm mailTest


