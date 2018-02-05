#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

typedef struct _huf {		//Ʈ�� ����ü
	char *word;
	int count;
	struct _huf *prev, *next;
}huf;

typedef struct _c_table {	//�ڵ����̺� ����ü
	char *name;
	char code[256];
	struct _c_table *link;
}c_table;

huf *c_head;
c_table *t_head=NULL;

char* file_name(int num);		//������ �̸��� �Է� �޴� �Լ�
long compress(char *c_name);	//���ڸ� �о���� �Լ�
int com_Rword(char *t);			//���ڸ� �о ���� ���ڰ� �����ϴ��� �˻��ϴ� �Լ�
void make_tree();				//Ʈ���� �����ϴ� �Լ�
void make_table(huf *name, char *code);	//Ʈ���� ������ �� �ڵ� ���̺��� ����� �Լ�
void tree_sort();				//Ʈ�� ������ �󵵼� ���� ���� �ϴ� �Լ�
void search(huf *temp, char buf[], huf *center);	//�ڵ� ���̺��� ���鶧 ���� ��ȸ�� Ʈ���� �˻��ϴ� �Լ�
void print_tree(huf *temp);		//Ʈ�� ��� �Լ�
void write_header(long E_cnt, char *input, char *ouput);	//���Ͽ� ����ϴ� �Լ�
int decompress(char *string);	//���� ���Ͽ��� ��� ������ �о� ���� �Լ�
void de_tree(char *t, huf* tree);	//����� ���Ϸ� ���� Ʈ���� ����� �Լ�
int de_Readword(FILE *rp, long f_len, char *out_name);	//����� ���Ͽ��� ���� ������ �ܾ ��ϵ� �κк��� �о���� �Լ�
long file_length(FILE *fp);		//������ ����Ʈ���� ���ϴ� �Լ�
void print_menu();


int main()
{
	int sel=0, val=0;
	long E_cnt=0;
	char buf[256];
	char input_name[20];
	char output_name[20];
	huf *t=NULL;

	while(1)
	{
		memset(input_name, (int)NULL, 20);
		memset(output_name, (int)NULL, 20);
		memset(buf, (int)NULL, 256);
		print_menu();		
		scanf("%d", &sel);
		switch(sel)
		{
		case 1:
			strcpy(input_name, file_name(1));		
			E_cnt=compress(input_name);				
			tree_sort();
			make_tree();
			search(c_head, buf, t);
			strcpy(output_name, file_name(3));
			write_header(E_cnt, input_name, output_name);
			break;
		case 2: 
			strcpy(input_name, file_name(5));
			decompress(input_name);
			break;
		case 3:
			exit(0);
		default:
			printf("�߸� �Է��ϼ̽��ϴ�!!\n");
		}
	}
	return 0;
}


/*################################################################################
#                                     ����                                       #
#       ������ ��ο��� �ܾ �ҷ��ͼ� com_Rword()�Լ��� ȣ���ϴ� �Լ�          #      
#################################################################################*/
long compress(char *c_name)
{
	FILE *fp;
	char *buff;
	long E_cnt=0;

	fp = fopen(c_name, "r");
	if(_access(c_name,0) !=0) {
		printf("������ �������� �ʽ��ϴ�!!!\n\n");
		memset(c_name, (int)NULL, 20);
		strcpy(c_name, file_name(2));
		compress(c_name);
		return E_cnt;
	}
	
	while(!feof(fp))
	{
		buff = '\0';
		fread(&buff, 1, 1, fp);
		if(buff == '\n')
			E_cnt++;
		if(feof(fp))
			break;
		com_Rword(buff);  //�Ѵܾ� �� �о com_Rword()ȣ��
	}
	fclose(fp);

	return E_cnt;
}



/*################################################################################
#                                     ����                                       #
#         �ҷ��� �ܾ��  �Ѵܾ �о �ܼ� ���Ḯ��Ʈ�� ����� �Լ�         #
################################################################################*/
int com_Rword(char *t)
{	
	int val = 0;
	huf *c_root=NULL;
	huf *c_leef=NULL;
	huf *c_search=NULL;

	if(c_head == (int)NULL)	{
		c_root = (huf*)malloc(sizeof(huf));
		c_leef = (huf*)malloc(sizeof(huf));
		c_root->prev = c_leef;
		c_root->next = NULL;
		c_root->count = 0;
		c_root->word = NULL;
		c_leef->word = t;
		c_leef->count = 1;
		c_leef->next = NULL;
		c_leef->prev = NULL;
		c_head=c_root;
		return 0;
	}

	for(c_search = c_head; c_search != NULL; c_search = c_search->next)	{
		if(c_search->prev->word == t) {
			c_search->prev->count = c_search->prev->count+ 1;
			val = 1;
			break;
		}
	}

	if(val == 0){
		c_root = (huf*)malloc(sizeof(huf));
		c_leef = (huf*)malloc(sizeof(huf));
		c_root->next = c_head;
		c_root->prev = c_leef;
		c_leef->word = t;
		c_leef->count =1;
		c_leef->next = NULL;
		c_leef->prev = NULL;
		c_head=c_root;
	}
	return 0;
}


/*################################################################################
#                                     ����                                       #
#                                Ʈ�� ���� �Լ�                                  #
#################################################################################*/
void tree_sort()
{
	huf *i, *j, *temp;

	for(i = c_head; i != NULL; i = i->next) {
		for(j=i->next; j != NULL; j = j->next) {
			if(i->prev->count > j->prev->count) {
				temp = i->prev;
				i->prev = j->prev;
				j->prev = temp;
			}
		}
	}
}



/*################################################################################
#                                     ����                                       #
#            �ܾ ���Ͽ��� ������ ��� ���� �� Ʈ���� ����� �Լ�              #
################################################################################*/
void make_tree()
{
	huf *temp, *front, *end;

	int val=0;
	for(temp = c_head; temp != NULL; temp = temp->next)
		val += temp->prev->count;

	end = temp;
	while(c_head->prev->count != val && c_head->next != NULL)
	{	
        temp = c_head;
		temp->word = NULL;

		front = c_head->next;
		temp->count = temp->prev->count + front->prev->count;
		temp->next = front->prev;

		front->prev = temp;
		c_head = front;
		
		temp = c_head;
		if(temp == end){
			temp->count =0;
			temp->word = NULL;
			temp->count = temp->prev->count + temp->next->count;
			break;
		}
		temp->count =0;
		tree_sort();
	}
	
	if(c_head->prev->word == NULL){
		temp = c_head;
		c_head = c_head->prev;
		free(temp);
	}	
}



/*################################################################################
#                                     ����                                       #
#                �ڵ����̺��� �����ϱ� ���� Ʈ���� �˻��ϴ� �Լ�                 #
#           �˻� �� ����� �ܾ NULL�� �ƴϸ� make_table �Լ��� ȣ��           #
################################################################################*/
void search(huf *tree, char buf[], huf *center)  //������ȸ
{
	if(tree->prev != NULL){
		strcat(buf, "0");
		center=tree;
		tree = tree->prev;
		search(tree, buf, center);
	}else if(tree->next != NULL){
		strcat(buf, "1");
		center=tree;
		tree=tree->next;
		search(tree, buf, center);
	}
	else if(tree->prev == NULL && tree->next == NULL)
	{
		if(tree->word != NULL) {
			make_table(tree, buf);  //table ����
			if(center->prev != NULL && center->prev->word == tree->word)
				center->prev = NULL;
			else if(center->next != NULL && center->next->word == tree->word)
				center->next = NULL;
			else {
				printf("program error #1\n");
				exit(0);
			}
			
			free(tree);
		}
		else if(tree->word == NULL)
		{
			if(center->prev != NULL && center->prev->count == tree->count)
				center->prev = NULL;
			else if(center->next != NULL && center->next->count == tree->count)
				center->next = NULL;
			else if(center == c_head){
				free(center);
				return;
			}				
			else {
				printf("program error #2\n");
				exit(0);
			}
			
			free(tree);
		}
		tree=c_head;
		memset(buf, (int)NULL, 256);
		search(tree, buf, center);
	}
	else {
		printf("program error #3\n");
		exit(0);
	}
}

/*################################################################################
#                                     ����                                       #
#                           �ڵ����̺��� ����� �Լ�                             #
################################################################################*/
void make_table(huf *r, char *code)
{
	c_table *s;
	
	if(t_head == (int)NULL)	{
		s = (c_table*)malloc(sizeof(c_table));
		memset(s->code, (int)NULL, 256);
		s->name = NULL;
		s->name = r->word;
		
		strcpy(s->code, code);
		s->link = NULL;
		t_head = s;
	}
	else {
		s = (c_table*)malloc(sizeof(c_table));
		memset(s->code, (int)NULL, 256);
		s->name = NULL;
		
		s->name = r->word;
		strcpy(s->code, code);
		s->link = t_head;
		t_head = s;
	}
	memset(code, (int)NULL, 256);
}


/*################################################################################
#                                     ����                                       #
#                    �ڵ����̺�� �ܾ ���ؼ� ���Ͽ� ����ϴ� �Լ�           #
################################################################################*/
void write_header(long E_cnt, char *input, char *output)
{	
	FILE *wp, *fp;
	c_table *s;
	unsigned char bit=0;
	char *buf;
	char *temp;
	char re_name[20];
	long count=0, len=0, i=0, f_len=0;
	int val;
	int bit_count=0;
	s=t_head;


	if(_access(output,0)==0)
	{
		printf("���� �̸��� ������ �����մϴ�!!\n");
		printf("���� ���ðڽ��ϱ�?(1:Yes or 2:No)\n");
		scanf("%d", &val);
		puts("");
		if(val==2) {
			printf("333\n");
			memset(re_name, (int)NULL, 20);			
			strcpy(re_name, file_name(4));
			write_header(E_cnt, input, re_name);
			return;
		}
	}
	fp = fopen(input, "rb");
	wp = fopen(output, "wb");		
	f_len = file_length(fp);
	len = f_len;

	fseek(fp, 0l, SEEK_SET);
	for(s=t_head; s !=NULL; s=s->link)
		count++;

	f_len = f_len-E_cnt;
	fprintf(wp, "%ld ", count);		//������ ����
	fprintf(wp, "%ld ", f_len);		//������ ����

	for(s=t_head; s !=NULL; s=s->link)
		fprintf(wp, "%c", s->name);		//�ܾ�

	fprintf(wp, "%c", NULL );
	for(s=t_head; s !=NULL; s=s->link)
		fprintf(wp, "%s ", s->code);	//�ڵ�

	buf = '\0';
	while(i<len)
	{		
		fread(&buf, 1, 1, fp);		
		if(i>=len)
			break;
		
		i++;
		for(s=t_head; s != NULL; s = s->link) {
			if(s->name == buf){
				temp = s->code;
				while(*temp != '\0'){
					if(*temp == '1')
						bit = bit<<1 | 1;
					else
						bit = bit<<1;
					
					bit_count++;
					if(bit_count >= 8) {
						fprintf(wp, "%c", bit);
						bit = 0;
						bit_count = 0;
					}
					
					if(*temp =='\0')
						break;
					temp++;
				}
				buf = '\0';
			}
		}

	}
	
	if(bit_count < 8) {
		for(count=bit_count; count<8; count++) {
			bit = bit << 1;				
		}
		fprintf(wp, "%c", bit);
	}
	
	f_len = file_length(fp);
	printf("���� �� : %ldByte\n", f_len);
	f_len = file_length(wp);
	printf("���� �� : %ldByte\n", f_len);
	fclose(wp);
	fclose(fp);
	
	s=t_head->link;
	while(s != NULL)
	{
		t_head->link = s->link;
		free(s);
		s = t_head->link;
	}
	free(t_head);	
}


/*################################################################################
#                                   ��������                                     #
#                 ���� ���Ͽ��� ��� ���� �� �ڵ带 �о���� �Լ�                #
#                  �о�� �ڵ� �� �ܾ�� �ܼ� ���Ḯ��Ʈ�� �����                #
################################################################################*/
int decompress(char *string)
{
	FILE *rp;
	huf *f, *b, *head=NULL;
	long count, i, f_len=0;
	char *buf;
	char temp[256];
	char re_name[20];

	rp = fopen(string, "rb");
	if(_access(string,0) !=0)
	{
		printf("������ �������� �ʽ��ϴ�!!!\n");
		memset(re_name, (int)NULL, 20);
		strcpy(re_name, file_name(6));
		printf("%s\n", re_name);
		decompress(re_name);
		return 0;
	}

	buf ='\0';
	fscanf(rp, "%ld", &count);	//������ ��
	fscanf(rp, "%ld", &f_len);	//������ ��ü����
	fread(&buf, 1, 1, rp);		//����
	buf='\0';
	
	c_head = (huf*)malloc(sizeof(huf)); //Ʈ���� ����� ���� 
	c_head->word = '\0';
	c_head->count = 0;
	c_head->next = c_head->prev = NULL;

	for(i=0; i<count; i++) {
		b = (huf*)malloc(sizeof(huf));	//������ ����ŭ ���� �Ҵ�
		fread(&buf, 1, 1, rp);			//���Ͽ��� �ܾ �Է¹޴´�.	
		if(head == NULL) {
			head = b;
			b->word = buf;
			b->count = 0;
			b->next = b->prev = NULL;
			f=head;
		}
		else{
			b->word = buf;
			b->count = 0;
			f->next = b;
			b->next = b->prev = NULL;
			f = b;
		}
		buf ='\0';
	}

	fread(&buf, 1, 1, rp); //����
	buf='\0';
	memset(temp, (int)NULL, 256);
	for(b=head; b != NULL;b = b->next) {		
		fscanf(rp, "%s", temp);		//�ڵ带 �д´�
		de_tree(temp, b);			//Ʈ�� ȣ��
		memset(temp, (int)NULL, 256);
	}
	for(b=head; b != NULL;) {
		f = b;
		b=b->next;
		free(f);
	}								

	memset(re_name, (int)NULL, 20);
	strcpy(re_name, file_name(7));
	de_Readword(rp, f_len, re_name);	//Ʈ�������� ������ ���� �ܾ���� ����� �κ��� �д´�
	return 0;
}


/*################################################################################
#                                  ��������                                      #
#             ���� ���Ͽ��� �о�� �ܾ�� �ڵ��� Ʈ���� ����� �Լ�            #
################################################################################*/
void de_tree(char *t, huf* tree)
{
	int val;
	huf *node, *under; 
	node  = c_head;
	under = node;

	while(*t != (int)NULL)
	{
		if(*t==(int)NULL)
			break;
		if(*t == '1')
		{
			if(node->next != NULL)	{
				node = node->next;
				under = node;
			}
			else{
				node = (huf*)malloc(sizeof(huf));
				node->word = '\0';
				under->next=node;
				under = node;
				node->next = node->prev=NULL;
			}
			val=1;
		} else {
			if(node->prev != NULL)
			{
				node = node->prev;
				under = node;
			}
			else
			{
				node = (huf*)malloc(sizeof(huf));
				node->word = '\0';
				under->prev = node;
				under = node;
				node->next = node->prev=NULL;
			}
			val=0;
		}
		t++;
	}

	node->word = tree->word;
}



/*################################################################################
#                                  ��������                                      #
#                     ����Ǳ� ���� ���� �ܾ�� ��ȯ�ϴ� �Լ�                    #
#                       ��Ʈ�� �˻��ؼ� Ʈ���� �˻��ϴ� �Լ�                     #
################################################################################*/
int de_Readword(FILE *rp, long f_len, char *out_name)
{
	FILE *wp;
	huf *s;
	long i=0, len=0;
	char buf;
	char temp;
	char re_name[20];
	unsigned int result;
	
	s = c_head;
	if(_access(out_name,0)==0)
	{
		printf("���� �̸��� ������ �����մϴ�!\n");
		printf("���� ���ðڽ��ϱ�? (1:Yes or 2:No)\n");
		scanf("%i", &i);
		puts("");
		if(i==2) {
			memset(re_name, (int)NULL, 20);
			strcpy(re_name, file_name(8));
			de_Readword(rp, f_len, re_name);
			return 0;
		}
		i=0;
	}

	wp = fopen(out_name, "w");
	fread(&buf, sizeof(char), 1, rp);
	temp=0;
	buf=0;

	while(1)
	{
		if(i>=f_len)
		  break;

		fread(&buf, sizeof(char), 1, rp);
		temp = buf;	
		
		result = temp & 0x80;
		if(result == 128){
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0){
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x40;
		if(result == 64) {
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x20;
		if(result == 32) {
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x10;
		if(result == 16) {
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;


		result = temp & 0x08;
		if(result == 8)	{
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x04;
		if(result == 4)	{
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x02;
		if(result == 2)	{
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;

		result = temp & 0x01;
		if(result == 1)	{
			if(s->next != (int)NULL)
				s = s->next;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}else if(result == 0) {
			if(s->prev != (int)NULL)
				s = s->prev;
			if(s->word != (int)NULL){
				fprintf(wp, "%c", s->word);
				s=c_head;
				i++;				
			}
		}
		if(i>=f_len)
			break;


		buf='\0';
		result=0;
	}
	
	len = file_length(rp);
	printf("���� �� : %ldByte\n", len);
	len = file_length(wp);
	printf("���� �� : %ldByte\n", len);
	fclose(rp);
	fclose(wp);
	return 0;
}


void print_menu() {
	printf("1. ���� \n");
	printf("2. �������� \n");
	printf("3. ���� \n");
	printf("Select Number : ");
}

long file_length(FILE *fp)
{
	long len;
	fseek(fp, 0l, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0l, SEEK_SET);
	return len;
}

void print_tree(huf* prt)
{
	if(prt){	
		print_tree(prt->prev);
		if(prt->word != NULL)
			printf("%c\n", prt->word);
		print_tree(prt->next);	
	}
}

char* file_name(int num)
{
	char string[20];
	char s[1];
	char *ret;

	memset(string, (int)NULL, 20);
	memset(s, (int)NULL, 1);

	switch(num){
	case 1:
	case 2:
		printf("������ ������ ��θ� �Է� �ϼ���\n");
		printf("[<��> c:\\\\abc.txt] : \n");	
		if(num == 1)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 3:
	case 4:
		printf("���� �� ������ ��θ� �Է��ϼ���\n");
		printf("[<��> c:\\\\abc.huf] : \n");		
		if(num == 4)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 5:
	case 6:
		printf("������ ������ ��θ� �Է��ϼ���\n");
		printf("[<��> c:\\\\abc.huf] : \n");
		if(num==5)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 7:
	case 8:
		printf("���� �� ������ ����� �Է��ϼ���\n");
		printf("[<��> c:\\\\abc.txt] : \n");
		if(num==8)
			getc(stdin);
		gets(string);
		puts("");
		break;
	default :
		printf("�߸� �Է��ϼ̽��ϴ�!!\n");
	}	

	ret = string;
	return ret;
}
