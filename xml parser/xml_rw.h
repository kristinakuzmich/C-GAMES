//------------------------------------------------------------------------------------//
/*             Реализация класса записи и парсинга XML документа.                     */
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

	void fill_buffer_tag(IXMLDOMDocument  *pDoc);//Заполнение tag_between тэгами
												//второго порядка.

	char m_head_tag[MAX_PATH]; //Главный тэг в котром лежат тэги от m_tag
	char m_tag[MAX_PATH];	//Имя тэга
	char m_const[MAX_PATH];	//Константа типа id=
	char m_const_result[MAX_PATH];//Значение константы ="Proc"
	char m_tag_text[MAX_PATH];	//Текст в тэге типа <proc>Hello world</proc>

	int loop;	//Значение определяющее количество заполненных ячеек в tag_between
    char *tag_between[MAX_PATH];//Массив основных тэгов которые в скоре будут сохранены
								// в m_heag_tag
	bool rezult; //Служит для проверки заполнен ли m_tag а если не заполнен то значит
				// что это информационный тэг типа <?xml ....?> и заполняет m_tag
	char NameRoot[100];//Сдесь будет храниться имя главного тэга XML файла <root>
public:
	XMLReadWrite(); //Инициализация.
	~XMLReadWrite(); //Деинициализатор

	/////////////////////////////////////////////////////////
	BOOL createRoot(char *name, IXMLDOMNode* &node);//Создание главного тэга.
	/*Создание элемента типа <proc>*/
	BOOL createElement(char *name, IXMLDOMNode* &node, IXMLDOMElement* &element);
	/*Вписывание текста в выбранный элемент типа <proc>Hello</proc>*/
	BOOL put_text(char *text, IXMLDOMNode* &node); 
	/*Вписывание атрибутов в выбранный тэг типа <proc id="cool proc"*/
	BOOL setAttribute(char *name, char *attr, IXMLDOMElement* &attrib);
	void save(char *filename);//Сохранение XML в файл
	////////////////////////////////////////////////////////
	BOOL readXML(char *filename, OPTIONS *opt);
	////////////////////////////////////////////////////////
	/*Данная функция может выполнять два вида действия, перед выполнением любого действия 
	нужно произвести инициализацию NameTag, его синтаксис таков для нижеприведенного XML:
		<root>
			<LOG id="0">
				<filename id="1"/>
				<text>log.txt</text>
			</LOG>
		</root>
	В NameTag нужно вписать имя тэга из которого ты хочешь узнать id или текст.
	Если мы хотим узнать значение id в подтэге filename в тэге LOG то нужно сделать
	так: read_step("/LOG/filename" "id");
	Если хотим узнать значение id в тэге LOG то: read_step("/LOG","id");
	Если хотим узнать текст в подтэге text в тэге LOG то: read_step("/LOG/text", "");
	Так как функция read_step типа char то ее возвращаемое значение нужно получить так:
	char buffer[100];
	strcpy(buffer, read_step("/LOG/filename", "id"));*/
	char* read_step(char *NameTag, char *child_attr);
	
};
