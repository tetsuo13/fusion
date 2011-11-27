#
# unexpand --tabs=4
#

CXX = g++
CXXFLAGS = -Wall -O2 -I. -I../ -I/opt/pgsql/include
CXXLIBS = -L/opt/pgsql/lib /opt/pgsql/lib/libpqxx.a -lpq -lpqxx
TARGET = ../../admin/index
RM = /bin/rm -f

all : Cal.o Layout.o Cgi.o FormInput.o main.o
	${CXX} -o ${TARGET} ../Cal.cpp ../Layout.cpp ../Cgi.cpp ../FormInput.cpp main.cpp ${CXXFLAGS} ${CXXLIBS}
	${RM} *.o
	cp functions.js ../../admin

Cal.o : ../Cal.cpp ../Cal.h
	${CXX} ${CXXFLAGS} -c -o $@ ../Cal.cpp

Layout.o : ../Layout.cpp ../Layout.h ../Cal.h
	${CXX} ${CXXFLAGS} -c -o $@ ../Layout.cpp

Cgi.o : ../Cgi.cpp ../Cgi.h
	${CXX} ${CXXFLAGS} -c -o $@ ../Cgi.cpp

FormInput.o : ../FormInput.cpp ../FormInput.h
	${CXX} ${CXXFLAGS} -c -o $@ ../FormInput.cpp

main.o : main.cpp
	${CXX} -o $@ Cal.o Layout.o Cgi.o FormInput.o $? ${CXXFLAGS} ${CXXLIBS}

clean :
	${RM} *.o core.* ${TARGET}
