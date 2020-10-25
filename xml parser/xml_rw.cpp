
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
		MessageBox(NULL,"������ CoInitialize",NULL,NULL);
		return; 
	}

	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
		IID_IXMLDOMDocument, (void**)&pXMLDoc);
	if(hr != S_OK) {
		MessageBox(NULL,"������ CoCreateInstance",NULL,NULL);
		return;
	}

	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pXDNode);
	if(hr != S_OK) {
		MessageBox(NULL,"������ QueryInterface",NULL,NULL);
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
			MessageBox(NULL,"������������ �������� ���.",NULL,NULL);
			return;
		}
		if(hr == E_ACCESSDENIED) {
			MessageBox(NULL,"��������� � ������ ���� �� ��������� ��������.",NULL,NULL);
			return;
		}
		if(hr == E_OUTOFMEMORY) {
			MessageBox(NULL,"����� � ����������� ������ ��� ������.",NULL,NULL);
			return;
		}
		MessageBox(NULL,"����������� ������ �������� �������.",NULL,NULL);
	}
}
/*------------------------------------------------------------------------------------*/
/*                                                                                    */
/*                  ���������� ������� ���������� XML �����.                          */
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
		MessageBox(NULL,"�� ���� ��������� ����",NULL,NULL);
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
	pNode->get_nodeName(&name); //�������� ��� ����
	DOMNodeType XMLtype;
	pNode->get_nodeType(&XMLtype);
	VARIANT vValue;
	strcpy(m_tag_text,"No data");

	/*��� ��������� �������� ��������������� ���� ������ m_head_tag ����� ��������
	  ����������������������������������������������������������������������������*/
	for(int i=0; i<=loop-1; i++)
	{
		if(!strcmp((_bstr_t)name, tag_between[i]))
		{
			strcpy(m_head_tag, tag_between[i]);
		}
	}

	switch ( XMLtype )
	{
		case NODE_TEXT:	//����� � ���� ���� <tag>Hello world</tag>
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
			pNode->get_nodeName(&name); //�������� ��� ����
			strcpy(m_tag,(_bstr_t)name);
		break; 
		
		case NODE_ELEMENT:
			pNode->get_nodeName(&name); //�������� ��� ����
			strcpy(m_tag,(_bstr_t)name);
			rezult = true;
		break;
	}
	//MessageBox(NULL, m_tag,NULL,NULL);

	//                                                              //
	//������� ��������� ���������� �� ��������� ��������������� ����//
	//         ���� <?xml version=1.0 encoding="ISO-8859-1"?>       //
	//                                                              //
	if(rezult != true)
	{
		pNode->get_nodeName(&name); //�������� ��� ����
		strcpy(m_tag,(_bstr_t)name);
	}
	rezult = false;
	/////////////////////////////////////////////////////////////////
	
	IterateAttibutes(pNode, opt); //������ ��� �������� 

	IXMLDOMNode *pNext = NULL;
	IXMLDOMNode *pChild = NULL;
	pNode->get_firstChild(&pChild); //������ ������� root ��� �������
	while( pChild ) //����������� �� ���������� �������� ���� root
	{
		IterateChildNodes(pChild, opt);
		pChild->get_nextSibling(&pNext); //�������� ���������� ��� ����������� � �������
										 //���� root
		pChild->Release();
		pChild = pNext;
	}
}

void XMLReadWrite::IterateAttibutes(IXMLDOMNode *pNode, OPTIONS *opt)
{
	IXMLDOMNamedNodeMap *pAttrs = NULL;

		if ( SUCCEEDED(pNode->get_attributes(&pAttrs)) && (pAttrs != NULL) )
		{ //�������� ��� �������� �������� ���� � pAttrs
			IXMLDOMNode *pChild1;
			VARIANT vValue;
			pAttrs->nextNode(&pChild1);

			while(pChild1) //����������� ���� �� ���������� ��� ���������
			{
				BSTR bstrName;
				BSTR text;
				pChild1->get_nodeName(&bstrName); //���������
				pChild1->get_nodeValue(&vValue); //�������� ���������

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
				pAttrs->nextNode(&pChild1); //�������� ���������
			}
			pAttrs->Release();
		}
}

void XMLReadWrite::fill_buffer_tag(IXMLDOMDocument  *pDoc)
{
	/*������� ��������� ������ tag_between ������ ������� �������, ������ ������
	�������� ����� ���� <root>, ��� ���� �� ������ �� ������ ��������� �����
	������� �������.*/
	IXMLDOMNodeList* nodes;
	IXMLDOMNode  *pNode;
	BSTR name;

	char nameRoot[100];
	//������ ��� �������� ���� XML �����. <root> � ���������� ��� � nameRoot
	if(pDoc->selectNodes((_bstr_t)"/*", &nodes) == S_OK)
	{
		while(nodes->nextNode(&pNode) != S_FALSE)
		{
			pNode->get_nodeName(&name);
		}
		strcpy(nameRoot, (_bstr_t)name);
		SysFreeString(name);
	}

	/*����������� ������ nameRoot � nameRoot1 ��� ������������ ������������� �� �
	SelectNodes ��� ����������� ����� ������� ������� � ������� ���� nameRoot*/
	char nameRoot1[100];
	sprintf(nameRoot1, "/%s/*", nameRoot);
	strcpy(NameRoot, nameRoot);

	/*����� ����� ������� ������� � ����������� �� � ���������� ������ tag_between, 
	��� ���� ���������������� ������� loop, ��� ��� ������������� � ������ ����� 
	��������� ��� ���� ��������� �� 1 ����� �� ����� ���������� ������ �������� 
	����������� ����� � tag_between*/
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

	pXMLDoc->selectNodes((_bstr_t)buffer,&Nl);//�������� ���
	Nl->get_item(0,&pNode1);

	if(child_attr != NULL)//���� �������� �� ���� �� �� ��� ����� ������ ��� ��������
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
	else//���� ������ �� ������ ����� ���� <root>Hello world!</root>
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
// ������� ���������� ���������, ������������ �������� ���������.  //
//-----------------------------------------------------------------//
void XMLReadWrite::CompareAttrib(char *head_tag, char *tag, char *ch_const, 
								 char *const_result, 
								 char *ch_text)
{
	char buffer[10000];
	wsprintf(buffer, "%s, %s, %s, %s, %s", m_head_tag, tag, ch_const, const_result, ch_text);
	//MessageBox(NULL,buffer,NULL,NULL);
}