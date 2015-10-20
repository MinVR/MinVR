
ASOURCES=attribute.cpp Cxml.cpp element.cpp utils.cpp XML.cpp
SOURCES=driver.cpp $(ASOURCES)
MVRSOURCES=VRDatum.h VRDatum.cpp VRDatumFactory.h VRDatumFactory.cpp VRDataCollection.h VRDataCollection.cpp VRDataIndex.h VRDataIndex.cpp
CFLAGS=-I/usr/include/malloc -ggdb -g3

TISOURCES=testindex.cpp VRDatum.cpp VRDatumFactory.cpp VRDataIndex.cpp attribute.cpp Cxml.cpp element.cpp utils.cpp XML.cpp

EXSOURCES=exercz.cpp $(MVRSOURCES) $(ASOURCES)

TPSOURCES=testproxy.cpp

exercz: $(EXSOURCES)
	g++ $(CFLAGS) -o exercz $(EXSOURCES)

testindex:   $(TISOURCES)
	g++ $(CFLAGS) -o testindex $(TISOURCES)

testproxy:   $(TPSOURCES)
	g++ $(CFLAGS) -o testproxy $(TPSOURCES)

xml_driver:  $(SOURCES)
	g++ $(CFLAGS) -o driver driver.cpp attribute.cpp Cxml.cpp element.cpp utils.cpp XML.cpp

