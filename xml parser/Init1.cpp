/*Сам XML находиться в ReadMe.txt*/

#include "stdafx.h"
#include <atlbase.h>
#include <tchar.h>
#import "msxml3.dll"
#include <windows.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>

#include "xml_rw.h"

int _tmain(int argc, _TCHAR* argv[])
{
	XMLReadWrite xml;
	OPTIONS opt;

	IXMLDOMNode *node = NULL;
	xml.createRoot("root",node);
	IXMLDOMElement *element = NULL;
	xml.createElement("buggy", node, element);
	IXMLDOMElement *element1 = NULL;
	xml.createElement("bamc", (IXMLDOMNode *&)element, element1);
	xml.put_text("COOL DENIS", (IXMLDOMNode *&)element1);
	xml.setAttribute("book","cool book", element1);
	xml.setAttribute("book1","cool boook1", element1);
	//xml.save("c:\\fff.xml");
	xml.readXML("c:\\data.xml", &opt);

	char nnn[100];
	strcpy(nnn,xml.read_step("LOG/server","name"));
	MessageBox(NULL,nnn,NULL,NULL);
	return 0;
}

