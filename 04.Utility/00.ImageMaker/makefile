# file		makefile
# date		2008/12/16
# author	kkamagui 
# brief		��ũ �̹����� �����ϴ� ���α׷��� �����ϴ� make ����

# �⺻������ ���带 ������ ���
all: ImageMaker.exe

# ImageMaker ����
ImageMaker.exe: ImageMaker.c
	gcc -o $@ $<
	cp ImageMaker.exe ../../ImageMaker.exe
# �ҽ� ������ ������ ������ ���� ����	
clean:
	rm -f ImageMaker.exe
