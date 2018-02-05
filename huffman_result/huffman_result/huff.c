#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

typedef struct _huf {		//트리 구조체
	char *word;
	int count;
	struct _huf *prev, *next;
}huf;

typedef struct _c_table {	//코드테이블 구조체
	char *name;
	char code[256];
	struct _c_table *link;
}c_table;

huf *c_head;
c_table *t_head=NULL;

char* file_name(int num);		//파일의 이름을 입력 받는 함수
long compress(char *c_name);	//문자를 읽어오는 함수
int com_Rword(char *t);			//문자를 읽어서 같은 문자가 존재하는지 검사하는 함수
void make_tree();				//트리를 생성하는 함수
void make_table(huf *name, char *code);	//트리를 생성한 후 코드 테이블을 만드는 함수
void tree_sort();				//트리 생성시 빈도수 별로 정렬 하는 함수
void search(huf *temp, char buf[], huf *center);	//코드 테이블을 만들때 중위 순회로 트리를 검사하는 함수
void print_tree(huf *temp);		//트리 출력 함수
void write_header(long E_cnt, char *input, char *ouput);	//파일에 기록하는 함수
int decompress(char *string);	//압축 파일에서 헤더 정보를 읽어 오는 함수
void de_tree(char *t, huf* tree);	//압축된 파일로 부터 트리를 만드는 함수
int de_Readword(FILE *rp, long f_len, char *out_name);	//압축된 파일에서 실제 파일의 단어가 기록된 부분부터 읽어오는 함수
long file_length(FILE *fp);		//파일의 바이트수를 구하는 함수
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
			printf("잘못 입력하셨습니다!!\n");
		}
	}
	return 0;
}


/*################################################################################
#                                     압축                                       #
#       파일의 경로에서 단어를 불러와서 com_Rword()함수를 호출하는 함수          #      
#################################################################################*/
long compress(char *c_name)
{
	FILE *fp;
	char *buff;
	long E_cnt=0;

	fp = fopen(c_name, "r");
	if(_access(c_name,0) !=0) {
		printf("파일이 존재하지 않습니다!!!\n\n");
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
		com_Rword(buff);  //한단어 씩 읽어서 com_Rword()호출
	}
	fclose(fp);

	return E_cnt;
}



/*################################################################################
#                                     압축                                       #
#         불러온 단어별로  한단어씩 읽어서 단순 연결리스트를 만드는 함수         #
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
#                                     압축                                       #
#                                트리 정렬 함수                                  #
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
#                                     압축                                       #
#            단어를 파일에서 끝까지 모두 읽은 후 트리를 만드는 함수              #
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
#                                     압축                                       #
#                코드테이블을 생성하기 위해 트리를 검색하는 함수                 #
#           검색 중 노드의 단어가 NULL이 아니면 make_table 함수를 호출           #
################################################################################*/
void search(huf *tree, char buf[], huf *center)  //중위순회
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
			make_table(tree, buf);  //table 생성
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
#                                     압축                                       #
#                           코드테이블을 만드는 함수                             #
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
#                                     압축                                       #
#                    코드테이블과 단어를 비교해서 파일에 기록하는 함수           #
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
		printf("같은 이름의 파일이 존재합니다!!\n");
		printf("덮어 쓰시겠습니까?(1:Yes or 2:No)\n");
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
	fprintf(wp, "%ld ", count);		//문자의 갯수
	fprintf(wp, "%ld ", f_len);		//파일의 길이

	for(s=t_head; s !=NULL; s=s->link)
		fprintf(wp, "%c", s->name);		//단어

	fprintf(wp, "%c", NULL );
	for(s=t_head; s !=NULL; s=s->link)
		fprintf(wp, "%s ", s->code);	//코드

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
	printf("압축 전 : %ldByte\n", f_len);
	f_len = file_length(wp);
	printf("압축 후 : %ldByte\n", f_len);
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
#                                   압축해제                                     #
#                 압축 파일에서 헤더 정보 및 코드를 읽어오는 함수                #
#                  읽어온 코드 및 단어로 단순 연결리스트를 만든다                #
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
		printf("파일이 존재하지 않습니다!!!\n");
		memset(re_name, (int)NULL, 20);
		strcpy(re_name, file_name(6));
		printf("%s\n", re_name);
		decompress(re_name);
		return 0;
	}

	buf ='\0';
	fscanf(rp, "%ld", &count);	//문자의 수
	fscanf(rp, "%ld", &f_len);	//파일의 전체길이
	fread(&buf, 1, 1, rp);		//공백
	buf='\0';
	
	c_head = (huf*)malloc(sizeof(huf)); //트리를 만들기 위해 
	c_head->word = '\0';
	c_head->count = 0;
	c_head->next = c_head->prev = NULL;

	for(i=0; i<count; i++) {
		b = (huf*)malloc(sizeof(huf));	//문자의 수만큼 동적 할당
		fread(&buf, 1, 1, rp);			//파일에서 단어를 입력받는다.	
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

	fread(&buf, 1, 1, rp); //공백
	buf='\0';
	memset(temp, (int)NULL, 256);
	for(b=head; b != NULL;b = b->next) {		
		fscanf(rp, "%s", temp);		//코드를 읽는다
		de_tree(temp, b);			//트리 호출
		memset(temp, (int)NULL, 256);
	}
	for(b=head; b != NULL;) {
		f = b;
		b=b->next;
		free(f);
	}								

	memset(re_name, (int)NULL, 20);
	strcpy(re_name, file_name(7));
	de_Readword(rp, f_len, re_name);	//트리구현이 끝나고 실제 단어들이 저장된 부분을 읽는다
	return 0;
}


/*################################################################################
#                                  압축해제                                      #
#             압축 파일에서 읽어온 단어와 코드들로 트리를 만드는 함수            #
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
#                                  압축해제                                      #
#                     압축되기 전의 실제 단어로 변환하는 함수                    #
#                       비트를 검사해서 트리를 검색하는 함수                     #
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
		printf("같은 이름의 파일이 존재합니다!\n");
		printf("덮어 쓰시겠습니까? (1:Yes or 2:No)\n");
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
	printf("해제 전 : %ldByte\n", len);
	len = file_length(wp);
	printf("해제 후 : %ldByte\n", len);
	fclose(rp);
	fclose(wp);
	return 0;
}


void print_menu() {
	printf("1. 압축 \n");
	printf("2. 압축해제 \n");
	printf("3. 종료 \n");
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
		printf("압축할 파일의 경로를 입력 하세요\n");
		printf("[<예> c:\\\\abc.txt] : \n");	
		if(num == 1)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 3:
	case 4:
		printf("압축 후 파일의 경로를 입력하세요\n");
		printf("[<예> c:\\\\abc.huf] : \n");		
		if(num == 4)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 5:
	case 6:
		printf("해제할 파일의 경로를 입력하세요\n");
		printf("[<예> c:\\\\abc.huf] : \n");
		if(num==5)
			getc(stdin);
		gets(string);
		puts("");
		break;
	case 7:
	case 8:
		printf("해제 후 파일의 경로을 입력하세요\n");
		printf("[<예> c:\\\\abc.txt] : \n");
		if(num==8)
			getc(stdin);
		gets(string);
		puts("");
		break;
	default :
		printf("잘못 입력하셨습니다!!\n");
	}	

	ret = string;
	return ret;
}
