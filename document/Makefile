#!/usr/bin/make -f

prefix=/usr/local
datadir=${prefix}/share
docdir=${datadir}/doc/gmonitor
release=0.0.13

all: doc-html doc-pdf

doc-html: man-html man-inst-html man-prog-html man-sys-html man-faq-html
doc-pdf:   man-pdf man-inst-pdf man-prog-pdf man-sys-pdf man-faq-pdf


man-html:
	mkdir -p gmonitor-doc-html; docbook2html -o gmonitor-doc-html/gmonitor-manual-html manual.xml && ./latin2acute.sh gmonitor-doc-html/gmonitor-manual-html/* && mkdir -p gmonitor-doc-html/gmonitor-manual-html/images; cp -v images/*.jpg gmonitor-doc-html/gmonitor-manual-html/images && chmod 0644 gmonitor-doc-html/gmonitor-manual-html/images/* && cd gmonitor-doc-html && tar cvzf gmonitor-manual-html-${release}.tar.gz gmonitor-manual-html && rm -r gmonitor-manual-html && cd ..

man-inst-html:
	mkdir -p gmonitor-doc-html; docbook2html -o gmonitor-doc-html/gmonitor-manual-instalacion-html manual-inst.xml && ./latin2acute.sh gmonitor-doc-html/gmonitor-manual-instalacion-html/* && mkdir -p gmonitor-doc-html/gmonitor-manual-instalacion-html/images; cp -v images/*.jpg gmonitor-doc-html/gmonitor-manual-instalacion-html/images && chmod 0644 gmonitor-doc-html/gmonitor-manual-instalacion-html/images/* && cd gmonitor-doc-html && tar cvzf gmonitor-manual-instalacion-html-${release}.tar.gz gmonitor-manual-instalacion-html && rm -r gmonitor-manual-instalacion-html && cd ..

man-prog-html:
	mkdir -p gmonitor-doc-html; docbook2html -o gmonitor-doc-html/gmonitor-manual-programacion-html manual-prog.xml && ./latin2acute.sh gmonitor-doc-html/gmonitor-manual-programacion-html/* && mkdir -p gmonitor-doc-html/gmonitor-manual-programacion-html/images; cp -v images/*.jpg gmonitor-doc-html/gmonitor-manual-programacion-html/images && chmod 0644 gmonitor-doc-html/gmonitor-manual-programacion-html/images/* && cd gmonitor-doc-html && tar cvzf gmonitor-manual-programacion-html-${release}.tar.gz gmonitor-manual-programacion-html && rm -r gmonitor-manual-programacion-html && cd ..

man-sys-html:
	mkdir -p gmonitor-doc-html; docbook2html -o gmonitor-doc-html/gmonitor-manual-sistema-html manual-syst.xml && ./latin2acute.sh gmonitor-doc-html/gmonitor-manual-sistema-html/* && mkdir -p gmonitor-doc-html/gmonitor-manual-sistema-html/images; cp -v images/*.jpg gmonitor-doc-html/gmonitor-manual-sistema-html/images && chmod 0644 gmonitor-doc-html/gmonitor-manual-sistema-html/images/* && cd gmonitor-doc-html && tar cvzf gmonitor-manual-sistema-html-${release}.tar.gz gmonitor-manual-sistema-html && rm -r gmonitor-manual-sistema-html && cd ..

man-faq-html:
	mkdir -p gmonitor-doc-html; docbook2html -o gmonitor-doc-html/gmonitor-manual-faq-html manual-faq.xml && ./latin2acute.sh gmonitor-doc-html/gmonitor-manual-faq-html/* && mkdir -p gmonitor-doc-html/gmonitor-manual-faq-html/images; cp -v images/*.jpg gmonitor-doc-html/gmonitor-manual-faq-html/images && chmod 0644 gmonitor-doc-html/gmonitor-manual-faq-html/images/* && cd gmonitor-doc-html && tar cvzf gmonitor-manual-faq-html-${release}.tar.gz gmonitor-manual-faq-html && rm -r gmonitor-manual-faq-html && cd ..
	
man-pdf:
	docbook2pdf -o gmonitor-doc-pdf manual.xml && mv gmonitor-doc-pdf/manual.pdf gmonitor-doc-pdf/gmonitor-manual-${release}.pdf

man-inst-pdf:
	docbook2pdf -o gmonitor-doc-pdf manual-inst.xml && mv gmonitor-doc-pdf/manual-inst.pdf gmonitor-doc-pdf/gmonitor-manual-instalacion-${release}.pdf

man-prog-pdf:
	docbook2pdf -o gmonitor-doc-pdf manual-prog.xml && mv gmonitor-doc-pdf/manual-prog.pdf gmonitor-doc-pdf/gmonitor-manual-programacion-${release}.pdf

man-sys-pdf:
	docbook2pdf -o gmonitor-doc-pdf manual-syst.xml && mv gmonitor-doc-pdf/manual-syst.pdf gmonitor-doc-pdf/gmonitor-manual-sistema-${release}.pdf

man-faq-pdf:
	docbook2pdf -o gmonitor-doc-pdf manual-faq.xml && mv gmonitor-doc-pdf/manual-faq.pdf gmonitor-doc-pdf/gmonitor-manual-faq-${release}.pdf

clean:
	rm -rf gmonitor-doc-html gmonitor-doc-pdf

install:
	tar x -C ${docdir} -v -z -f gmonitor-doc-html/gmonitor-manual-html-${release}.tar.gz
	tar x -C ${docdir} -v -z -f gmonitor-doc-html/gmonitor-manual-sistema-html-${release}.tar.gz
	tar x -C ${docdir} -v -z -f gmonitor-doc-html/gmonitor-manual-instalacion-html-${release}.tar.gz
	tar x -C ${docdir} -v -z -f gmonitor-doc-html/gmonitor-manual-programacion-html-${release}.tar.gz
	tar x -C ${docdir} -v -z -f gmonitor-doc-html/gmonitor-manual-faq-html-${release}.tar.gz
	cp -rv gmonitor-doc-pdf ${docdir}
