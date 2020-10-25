//------------------------------------------------------------------------------------//
/*             ���������� ������ ������ � �������� XML ���������.                     */
//------------------------------------------------------------------------------------//

typedef struct
{
	char *name;
}OPTIONS;

#pragma once
class XMLReadWrite
{
	HRESULT hr;
	IXMLDOMDocument  *pXMLDoc;
	IXMLDOMNode  *pXDNode;

	void IterateChildNodes(IXMLDOMNode *pNode, OPTIONS *opt);
	void IterateAttibutes(IXMLDOMNode *pNode, OPTIONS *opt);
	void CompareAttrib(char *head_tag ,char *tag, char *ch_const, char *const_result,
					   char *ch_text);

	void fill_buffer_tag(IXMLDOMDocument  *pDoc);//���������� tag_between ������
												//������� �������.

	char m_head_tag[MAX_PATH]; //������� ��� � ������ ����� ���� �� m_tag
	char m_tag[MAX_PATH];	//��� ����
	char m_const[MAX_PATH];	//��������� ���� id=
	char m_const_result[MAX_PATH];//�������� ��������� ="Proc"
	char m_tag_text[MAX_PATH];	//����� � ���� ���� <proc>Hello world</proc>

	int loop;	//�������� ������������ ���������� ����������� ����� � tag_between
    char *tag_between[MAX_PATH];//������ �������� ����� ������� � ����� ����� ���������
								// � m_heag_tag
	bool rezult; //������ ��� �������� �������� �� m_tag � ���� �� �������� �� ������
				// ��� ��� �������������� ��� ���� <?xml ....?> � ��������� m_tag
	char NameRoot[100];//����� ����� ��������� ��� �������� ���� XML ����� <root>
public:
	XMLReadWrite(); //�������������.
	~XMLReadWrite(); //���������������

	/////////////////////////////////////////////////////////
	BOOL createRoot(char *name, IXMLDOMNode* &node);//�������� �������� ����.
	/*�������� �������� ���� <proc>*/
	BOOL createElement(char *name, IXMLDOMNode* &node, IXMLDOMElement* &element);
	/*���������� ������ � ��������� ������� ���� <proc>Hello</proc>*/
	BOOL put_text(char *text, IXMLDOMNode* &node); 
	/*���������� ��������� � ��������� ��� ���� <proc id="cool proc"*/
	BOOL setAttribute(char *name, char *attr, IXMLDOMElement* &attrib);
	void save(char *filename);//���������� XML � ����
	////////////////////////////////////////////////////////
	BOOL readXML(char *filename, OPTIONS *opt);
	////////////////////////////////////////////////////////
	/*������ ������� ����� ��������� ��� ���� ��������, ����� ����������� ������ �������� 
	����� ���������� ������������� NameTag, ��� ��������� ����� ��� ���������������� XML:
		<root>
			<LOG id="0">
				<filename id="1"/>
				<text>log.txt</text>
			</LOG>
		</root>
	� NameTag ����� ������� ��� ���� �� �������� �� ������ ������ id ��� �����.
	���� �� ����� ������ �������� id � ������� filename � ���� LOG �� ����� �������
	���: read_step("/LOG/filename" "id");
	���� ����� ������ �������� id � ���� LOG ��: read_step("/LOG","id");
	���� ����� ������ ����� � ������� text � ���� LOG ��: read_step("/LOG/text", "");
	��� ��� ������� read_step ���� char �� �� ������������ �������� ����� �������� ���:
	char buffer[100];
	strcpy(buffer, read_step("/LOG/filename", "id"));*/
	char* read_step(char *NameTag, char *child_attr);
	
};
