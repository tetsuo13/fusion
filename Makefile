#
# unexpand --tabs=4
#

CXX = g++
CXXFLAGS = -Wall -O2 -I. -I/opt/pgsql/include
CXXLIBS = -L/opt/pgsql/lib /opt/pgsql/lib/libpqxx.a -lpq -lpqxx
INDEX_TARGET = ../index
RSS_TARGET = ../feed.xml
RM = /bin/rm -f

all : index rss
	${RM} *.o

#index : common.o Mail.o Cal.o Layout.o Cgi.o FormInput.o main.o
#	${CXX} -o ${INDEX_TARGET} common.o Mail.o Cal.o Layout.o Cgi.o FormInput.o main.o ${CXXFLAGS} ${CXXLIBS}
index : common.o Mail.o Cal.o Layout.o Cgi.o FormInput.o main.o
	${CXX} -o ${INDEX_TARGET} common.cpp Mail.cpp Cal.cpp Layout.cpp Cgi.cpp FormInput.cpp main.cpp ${CXXFLAGS} ${CXXLIBS}

rss : Cal.o Layout.o Cgi.o rss.o
	${CXX} -o ${RSS_TARGET} Cal.cpp Layout.cpp Cgi.cpp rss.cpp ${CXXFLAGS} ${CXXLIBS}

rss.o : rss.cpp
	${CXX} -o $@ Cal.o Layout.o Cgi.o rss.cpp ${CXXFLAGS} ${CXXLIBS}

common.o : common.cpp common.h
	${CXX} ${CXXFLAGS} -c -o $@ common.cpp

Mail.o : Mail.cpp Mail.h
	${CXX} ${CXXFLAGS} -c -o $@ Mail.cpp

Cal.o : Cal.cpp Cal.h
	${CXX} ${CXXFLAGS} -c -o $@ Cal.cpp

Layout.o : Layout.cpp Layout.h Cal.h
	${CXX} ${CXXFLAGS} -c -o $@ Layout.cpp

Cgi.o : Cgi.cpp Cgi.h
	${CXX} ${CXXFLAGS} -c -o $@ Cgi.cpp

FormInput.o : FormInput.cpp FormInput.h
	${CXX} ${CXXFLAGS} -c -o $@ FormInput.cpp

main.o : main.cpp
	${CXX} -o $@ common.o Mail.o Cal.o Layout.o Cgi.o FormInput.o main.cpp ${CXXFLAGS} ${CXXLIBS}

clean :
	${RM} *.o core.* ${INDEX_TARGET} ${RSS_TARGET}
