
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <atlbase.h>
#include <tchar.h>
#import "msxml3.dll"
#include <windows.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>

#include "xml_rw.h"

#pragma comment (lib, "msxml2.lib");

XMLReadWrite::XMLReadWrite()
{
	loop = 0;
	rezult = false;
	hr = CoInitialize(NULL); 
	if(hr != S_OK) {
		MessageBox(NULL,"Ошибка CoInitialize",NULL,NULL);
		return; 
	}

	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
		IID_IXMLDOMDocument, (void**)&pXMLDoc);
	if(hr != S_OK) {
		MessageBox(NULL,"Ошибка CoCreateInstance",NULL,NULL);
		return;
	}

	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pXDNode);
	if(hr != S_OK) {
		MessageBox(NULL,"Ошибка QueryInterface",NULL,NULL);
		return;
	}
}

XMLReadWrite::~XMLReadWrite()
{
	pXDNode->Release();
	pXDNode = NULL;
	pXMLDoc->Release();
	pXMLDoc = NULL;
	CoUninitialize();
}

BOOL XMLReadWrite::createRoot(char *name, IXMLDOMNode* &node)
{
	pXMLDoc->createNode(CComVariant(NODE_ELEMENT),(_bstr_t)name, (_bstr_t)"", &node);
	IXMLDOMNode *outnewChild = NULL;
	pXMLDoc->appendChild(node, &outnewChild);
	return TRUE;
}

BOOL XMLReadWrite::createElement(char *name, IXMLDOMNode* &node, IXMLDOMElement* &element)
{
	IXMLDOMNode *outnewChild = NULL;
	pXMLDoc->createElement((_bstr_t)name, &element);
	node->appendChild(element, &outnewChild);
	return TRUE;
}

BOOL XMLReadWrite::put_text(char *text, IXMLDOMNode* &node)
{
	node->put_text((_bstr_t)text);
	return TRUE;
}

BOOL XMLReadWrite::setAttribute(char *name, char *attr, IXMLDOMElement* &attrib)
{
	attrib->setAttribute((_bstr_t)name, CComVariant(attr));
	return TRUE;
}

void XMLReadWrite::save(char *filename)
{
	hr = pXMLDoc->save((_variant_t)filename);
	if(hr != S_OK)
	{
		if(hr == E_INVALIDARG ) {
			MessageBox(NULL,"Неправильное файловое имя.",NULL,NULL);
			return;
		}
		if(hr == E_ACCESSDENIED) {
			MessageBox(NULL,"Сохранять в данный файл не разрешено системой.",NULL,NULL);
			return;
		}
		if(hr == E_OUTOFMEMORY) {
			MessageBox(NULL,"Место в оперативной памяти все занято.",NULL,NULL);
			return;
		}
		MessageBox(NULL,"Неизвестная ошибка файловой системы.",NULL,NULL);
	}
}
/*------------------------------------------------------------------------------------*/
/*                                                                                    */
/*                  Реализация функций считывания XML файла.                          */
/*                                                                                    */
/*                                                                                    */
/*------------------------------------------------------------------------------------*/
BOOL XMLReadWrite::readXML(char *filename, OPTIONS *opt)
{
	_variant_t varOut((bool)TRUE);
	VARIANT_BOOL vBool;
	CComVariant vFile(filename);
	pXMLDoc->load(vFile,&vBool);
	if ( vBool == VARIANT_FALSE)
		MessageBox(NULL,"Не могу загрузить файл",NULL,NULL);
	fill_buffer_tag(pXMLDoc);
	/*for(int l=0; l<=loop-1; l++)
	{
		MessageBox(NULL,(char*)tag_between[l],NULL,NULL);
	}*/
	IterateChildNodes(pXDNode, opt);

	return TRUE;
}

void XMLReadWrite::IterateChildNodes(IXMLDOMNode *pNode,OPTIONS *opt)
{
	BSTR name;
	pNode->get_nodeName(&name); //Получаем имя тэга
	DOMNodeType XMLtype;
	pNode->get_nodeType(&XMLtype);
	VARIANT vValue;
	strcpy(m_tag_text,"No data");

	/*При начальной закрузке информационного тэга массив m_head_tag будет заполнен
	  ММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММММ*/
	for(int i=0; i<=loop-1; i++)
	{
		if(!strcmp((_bstr_t)name, tag_between[i]))
		{
			strcpy(m_head_tag, tag_between[i]);
		}
	}

	switch ( XMLtype )
	{
		case NODE_TEXT:	//Текст в тэге типа <tag>Hello world</tag>
			pNode->get_nodeValue(&vValue);
			switch (vValue.vt)
			{
				case VT_BSTR:
					wsprintf(m_tag_text,"%S",V_BSTR(&vValue));
					//MessageBox(NULL, m_tag_text,NULL,NULL);
					CompareAttrib(m_head_tag,m_tag,m_const,m_const_result,m_tag_text);
				break; 
			}
			break;

		case NODE_ATTRIBUTE:
			pNode->get_nodeName(&name); //Получаем имя тэга
			strcpy(m_tag,(_bstr_t)name);
		break; 
		
		case NODE_ELEMENT:
			pNode->get_nodeName(&name); //Получаем имя тэга
			strcpy(m_tag,(_bstr_t)name);
			rezult = true;
		break;
	}
	//MessageBox(NULL, m_tag,NULL,NULL);

	//                                                              //
	//Участок программы отвечающий за обработку информационного тэга//
	//         типа <?xml version=1.0 encoding="ISO-8859-1"?>       //
	//                                                              //
	if(rezult != true)
	{
		pNode->get_nodeName(&name); //Получаем имя тэга
		strcpy(m_tag,(_bstr_t)name);
	}
	rezult = false;
	/////////////////////////////////////////////////////////////////
	
	IterateAttibutes(pNode, opt); //узнаем его атрибуты 

	IXMLDOMNode *pNext = NULL;
	IXMLDOMNode *pChild = NULL;
	pNode->get_firstChild(&pChild); //делаем главный root тэг текущим
	while( pChild ) //выполняется до завершения главного тэга root
	{
		IterateChildNodes(pChild, opt);
		pChild->get_nextSibling(&pNext); //слудущий внутренний тэг находящейся в главном
										 //тэге root
		pChild->Release();
		pChild = pNext;
	}
}

void XMLReadWrite::IterateAttibutes(IXMLDOMNode *pNode, OPTIONS *opt)
{
	IXMLDOMNamedNodeMap *pAttrs = NULL;

		if ( SUCCEEDED(pNode->get_attributes(&pAttrs)) && (pAttrs != NULL) )
		{ //получаем все атрибуты текущего тэга в pAttrs
			IXMLDOMNode *pChild1;
			VARIANT vValue;
			pAttrs->nextNode(&pChild1);

			while(pChild1) //повторяется пока не закончатся все константы
			{
				BSTR bstrName;
				BSTR text;
				pChild1->get_nodeName(&bstrName); //константа
				pChild1->get_nodeValue(&vValue); //значение константы

				switch ( vValue.vt )
				{
					case VT_BSTR:
						wsprintf(m_const_result,"%S",V_BSTR(&vValue));
						break;
					default:
						strcpy(m_const_result,"Unsupport type");
						break;
				}
				strcpy(m_const,(_bstr_t)bstrName);
				/*MessageBox(NULL, m_const,NULL,NULL);
				MessageBox(NULL,m_const_result,NULL,NULL);*/
				CompareAttrib(m_head_tag,m_tag,m_const,m_const_result,m_tag_text);
				strcpy(m_const,"No data");
				strcpy(m_const_result,"No data");
				SysFreeString(bstrName);
				pChild1->Release();
				pAttrs->nextNode(&pChild1); //слудущая константа
			}
			pAttrs->Release();
		}
}

void XMLReadWrite::fill_buffer_tag(IXMLDOMDocument  *pDoc)
{
	/*Функция заполняет массив tag_between тэгами второго порядка, тоесть тэгами
	стоящими после тэга <root>, при этом не заходя во внутрь найденных тэгов
	второго порядка.*/
	IXMLDOMNodeList* nodes;
	IXMLDOMNode  *pNode;
	BSTR name;

	char nameRoot[100];
	//Узнаем имя главного тэга XML файла. <root> И записываем его в nameRoot
	if(pDoc->selectNodes((_bstr_t)"/*", &nodes) == S_OK)
	{
		while(nodes->nextNode(&pNode) != S_FALSE)
		{
			pNode->get_nodeName(&name);
		}
		strcpy(nameRoot, (_bstr_t)name);
		SysFreeString(name);
	}

	/*Форматируем строку nameRoot в nameRoot1 для последующего использования ее в
	SelectNodes для опознования тэгов второго порядка в главном тэге nameRoot*/
	char nameRoot1[100];
	sprintf(nameRoot1, "/%s/*", nameRoot);
	strcpy(NameRoot, nameRoot);

	/*Поиск тэгов второго порядка и копирования их в одномерный массив tag_between, 
	при этом инкрементируется счетчик loop, при его использовании в другой части 
	программы его надо уменьшить на 1 тогда он будет показывать точное значение 
	заполненных ячеек в tag_between*/
	if(pDoc->selectNodes((_bstr_t)nameRoot1, &nodes) == S_OK)
	{
		while(nodes->nextNode(&pNode) != S_FALSE)
		{
			pNode->get_nodeName(&name);
			strcpy(nameRoot, (_bstr_t)name);
			tag_between[loop]=new char[strlen(nameRoot)+1]; 
			strcpy(tag_between[loop], nameRoot);
			loop++;
		}
		SysFreeString(name);
	}
	ZeroMemory(nameRoot,100);
	ZeroMemory(nameRoot1,100);
}

char* XMLReadWrite::read_step(char *NameTag, char *child_attr)
{
	char buffer[MAX_PATH];
	sprintf(buffer, "%s/%s", NameRoot, NameTag);

	IXMLDOMNodeList *Nl;
	IXMLDOMNode *pNode1;
	IXMLDOMNode *N;

	pXMLDoc->selectNodes((_bstr_t)buffer,&Nl);//Выбираем тэг
	Nl->get_item(0,&pNode1);

	if(child_attr != NULL)//Если параметр не пуст то по его имени узнаем его значение
	{
		if(strcmp(child_attr,""))
		{
			IXMLDOMNamedNodeMap *pMap;
			pNode1->get_attributes(&pMap);
			pMap->getNamedItem((_bstr_t)child_attr,&N);
			VARIANT vvv;
			N->get_nodeValue(&vvv);
			ZeroMemory(buffer, MAX_PATH);
			wsprintf(buffer,"%S",V_BSTR(&vvv));
		}
		else
			goto num;
	}
	else//Если пустой то узнаем текст типа <root>Hello world!</root>
	{
num:
		BSTR text_tag;
		pNode1->get_text(&text_tag);
		ZeroMemory(buffer, MAX_PATH);
		strcpy(buffer, (_bstr_t)text_tag);
		SysFreeString(text_tag);
	}

	return buffer;
}
//-----------------------------------------------------------------//
// Функция заполнения структуры, определяющая свойства программы.  //
//-----------------------------------------------------------------//
void XMLReadWrite::CompareAttrib(char *head_tag, char *tag, char *ch_const, 
								 char *const_result, 
								 char *ch_text)
{
	char buffer[10000];
	wsprintf(buffer, "%s, %s, %s, %s, %s", m_head_tag, tag, ch_const, const_result, ch_text);
	//MessageBox(NULL,buffer,NULL,NULL);
}