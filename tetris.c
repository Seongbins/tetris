#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	start_color();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		createRankList();//LinkedList 만드려고 호출
		clear();//화면을 지워줌
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;//MENU_RANK 선택시 rank함수로
		case MENU_EXIT: exit=1; break;
		case MENU_REC: recommendedPlay(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	
	// next block[2] 태두리를 그린다.
	move(8,WIDTH+10);
	DrawBox(9,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
    case 'p':
    case 'P':
        command = PAUSE;
        break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	int i, k, temp;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	case ' ':
		for(i=0;i<HEIGHT;i++)
		{
			k=CheckToMove(field,nextBlock[0],blockRotate,blockY+i,blockX);
			if(k==0)
				break;
		}
		temp=blockY+i-1;
		drawFlag=1;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);

	if(command==' ')
	{
		blockY=temp;
		if(blockY<=1)
			gameOver=1;
		AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		k=DeleteLine(field);
		score=score+k+100;//스페이스를 통해 hard drop했을 경우 Bonus 100 point
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;
		blockRotate=0; blockY=-1; blockX=WIDTH/2-2;
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		timed_out=0;
	}
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(6,COLOR_CYAN,COLOR_BLACK);
	init_pair(7,COLOR_WHITE,COLOR_BLACK);

	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				attron(COLOR_PAIR(nextBlock[1]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[1]+1));
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		//Next Block[2]를 그린다.
		move(i+10, WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1){
				attron(A_REVERSE);
				attron(COLOR_PAIR(nextBlock[2]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[2]+1));
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	//char str[10]="hello";
	
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(6,COLOR_CYAN,COLOR_BLACK);
	init_pair(7,COLOR_WHITE,COLOR_BLACK);
	/*기본적인 구조가 DrawBlock->DrawShadow가 반복되는 구조로 되어있다.
	즉 이는 무한하게 돌아가게 된다. 하지만 우리가 원하는 것은 한 번 시행할 때 DrawBlock->DrawShadow->DrawBlock을 하고 끝내는 것을 원한다. 따라서
	tile이 '/'이 아닐 때, 즉 DrawShadow를 수행하지 않았을 때에만
	DrawShadow를 호출함으로써 DrawShadow가 한 버만 수행되도록 한다.*/
	if(tile==' ')
		DrawShadow(y, x, blockID, blockRotate);


	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				attron(COLOR_PAIR(blockID+1));
				printw("%c",tile);
				attroff(COLOR_PAIR(blockID+1));
				attroff(A_REVERSE);
				refresh();
                //move(HEIGHT,WIDTH+1);
			}
		}	
	
	move(HEIGHT,WIDTH+1);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('o');
	//addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
	//	addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	//	addch(ACS_HLINE);
	//addch(ACS_LRCORNER);
	addch('o');
}

void play(){
	int command;
	int i=0;
	int speed;
	clear();
	InitTetris();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
		
	do{
		if(timed_out==0){
			if(score==0)
				alarm(1);
			else
			{
				speed=score*50;
				if(speed>=900000)
					speed=900000;
				ualarm(1000000-speed,0);
			}
			//usleep(1000000);
            timed_out=1;
		}

		command = GetCommand();
		if(command==PAUSE)
        {
            getch();
        }
        if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	NewRecord(score);
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	//blockY와 blockX에는 바꾸고 싶어하는 위치가 들어왔다.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			//바꾸고 싶은 위치의 블록에 색칠된 것이
			if(block[currentBlock][blockRotate][i][j]==1)
			{
				//이미 쌓여있는 블록과 같다면 0 return
				if(f[blockY+i][blockX+j]==1)
					return 0;
				//y범위 밖이라면 0 return
				if(blockY+i>=HEIGHT)
					return 0;
				//x범위 밖이라면 0 return
				if(blockX+j<0 || blockX+j>=WIDTH)
					return 0;
			}
		}
	}
	//제대로된 범위(위에서 안 걸러졌다면) 1 return
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j, k;
	/*어떠한 command를 수행하기 이전에 그려져있던 block을 다시 '.'으로 바꿔주어야 하므로,
	command 이전의 상태로 만들어 주었다.*/

	switch(command)
	{
	case(KEY_RIGHT) :
		blockX=blockX-1;
		break;
	case(KEY_LEFT) :
		blockX=blockX+1;
		break;
	case(KEY_DOWN) :
		blockY=blockY-1;
		break;
	case(KEY_UP) :
		blockRotate=(blockRotate+3)%4;
		break;
	case ' ':
		break;
	}
	//command 이전의 상태에서 채워져 있던 것을 다시 '.'으로 바꾼다.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j]==1 && i+blockY>=0)
			{
				move(i+blockY+1,j+blockX+1);
				printw(".");
			}
		}
	}
	/*여기는 shadow와 관련이 있는 파트. shadow를 구현하고자 할 때, command 이전의
	shadow가 계속 남아있는 현상이 발생하여, command 이전 상태일 때의 shadow를 없애야
	할 필요가 있다.*/
	for(i=0;i<HEIGHT;i++)
	{
		j=CheckToMove(field,currentBlock,blockRotate,blockY+i,blockX);
		if(j==0)
			break;
	}
	//command 이전의 상태일 때의 그림자를 다시 '.' 상태로 바꿔준다.
	for(k=0;k<4;k++)
		for(j=0;j<4;j++)
			{
				if(block[currentBlock][blockRotate][k][j]==1)
				{
					move(blockY+i-1+k+1,blockX+j+1);
					printw(".");
				}
			}
	//다시 command상태로 바꿔준다. 이제 우리가 하고자 하는 일을 위해서.
	switch(command)
	{
	case(KEY_RIGHT) :
		blockX=blockX+1;
		break;
	case(KEY_LEFT) :
		blockX=blockX-1;
		break;
	case(KEY_DOWN) :
		blockY=blockY+1;
		break;
	case(KEY_UP) :
		blockRotate=(blockRotate+1)%4;
		break;
	case ' ' :
		blockY=blockY+i-1;
		break;
	}
	//command 받은 block을 그려주도록 DrawBlock을 호출한다.
	DrawBlock(blockY,blockX,currentBlock,blockRotate,' ');

}

void BlockDown(int sig){
	//printf("hi ");
	int k=0;
	RecNode* root;
	//recX=0; recY=0; recS=0; recR=0;
	switch(nextBlock[1])
	{
		case 0 : k=17; break;
		case 1 : k=34; break;
		case 2 : k=34; break;
		case 3 : k=34; break;
		case 4 : k=9; break;
		case 5 : k=17; break;
		case 6 : k=17; break;
	}
	//root=CreateNode(1,0,0,0,0,36);
	//copyField(field,root->recField);
	k=0;
	//아래로 내려갈 수 있는지를 CheckToMove를 통해 확인
	if( CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==1)
	{
	//내려갈 수 있다면 내려가자! 그리고 DrawChange를 통해 기존 잔상 삭제
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
		//if(count!=0)
		//	DrawRecommend(recY,recX,nextBlock[0],recR);
	}
	//만약 내려갈 수 없는 상태라면
	else
	{
		//근데 맨 위라면 gameover!
		if(blockY==-1)
			gameOver=1;
		//내려갈 수 없는 상태이므로 현재 block을 Field에 쌓기위해 함수 호출
		k=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score=score+k;
		//block을 필드에 쌓았으므로, Line을 지울 수 있는 것이 있는지 확인
		k=DeleteLine(field);
		//지워진 Line에 따른 점수만큼 score 증가시키기
		score=score+k;
		//다음에 쌓으려던 block을 현재블록으로, 다다음에 쌓으려던 블록을 다음 블록으로,
		//그리고 새로운 다다음 블록을 rand함수를 통해 설정
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;
		//recommended(root);////////////////////////
		blockRotate=0; blockY=-1; blockX=WIDTH/2-2;
		//바뀐 NextBlock들을 그리기
		//recX=0; recY=0; recR=0;
		recS=0;
		DrawNextBlock(nextBlock);
		//Score를 print하기
		PrintScore(score);
		//바뀐 Field를 그리기
		DrawField();
        root=CreateNode(1,0,0,0,0,k);
		copyField(field,root->recField);
		recommended(root);
		count++;
		//recX=0;recY=0;recS=0;recR=0;
		root=NULL;
	}
	//move(10,5); printw("A");
	//usleep(500*1000);
	//timed_out을 0으로 재설정함으로써 1초 뒤에 다시 떨어지도록
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int touched=0;//닿은 면적을 세는 변수
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j]==1)
			{
				//block이 채워진 부분을 field에 채워 넣는다.
				f[blockY+i][blockX+j]=1;
				//만약 채워진 부분이 field의 바닥이라면 touched를 1 증가시킨다.
				if((blockY+i)==HEIGHT-1)
					touched++;
				else if(f[blockY+i+1][blockX+j]==1)
					touched++;
			}
		}
	//score 계산을 위해서, 기존의 score에다가 touched*10을 더한다.
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, flag, y, x;
	int dl=0;
	//맨 밑줄부터 꽉찬 줄이 있나 살펴본다.
	for(i=HEIGHT;i>-1;i--)
	{
		flag=0;
		for(j=0;j<WIDTH;j++)
		{	
			//꽉차있지 않은 것이 있다면 break
			if(f[i][j]==0)
				break;
			//각 가로줄에 있는 칸마다 차있으면 flag 증가.
			flag++;
		}
		//그 줄이 꽉차있다면(flag가 어떤 줄에 차 있는 칸의 수이므로 WIDTH와 같으면 차있는 것임.)
		if(flag==WIDTH)
		{
			//꽉 차있는 줄 1줄 증가.
			dl++;
			//이 줄 위에 있는 줄들을 다 한 줄씩 아래로 내리도록 한다.
			for(y=i;y>-1;y--)
				for(x=0;x<WIDTH;x++)
					f[y][x]=f[y-1][x];
			//아래로 한 줄씩 내렸으므로, 여기서 한 줄 위로 올라가면 한 줄을 건너뛰게
			//되므로, 이를 방지하기 위하여. 
			i++;
		}
	}
	//지워진 line^2 *100만큼 score를 증가시키기 위해 이를 return한다.
	return dl*dl*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int i, k;

	for(i=0;i<HEIGHT;i++)
	{
		//현재의 Block을 계속 한 칸씩 내렸을 때, 더 이상 내릴수 없는 곳의 좌표를 판단
		k=CheckToMove(field,nextBlock[0],blockRotate,y+i,x);
		//더 이상 내릴 수 없는 상태일 경우 break
		if(k==0)
			break;
	}
	//더 이상 내릴 수 없는 그 상태를 '/' tile로 DrawBlock함수로 넘긴다.
	DrawBlock(y+i-1,x,nextBlock[0],blockRotate,'/');
}

void createRankList(){
	FILE *fp;
	char str[NAMELEN];
	int sco, count=0;
	int i=0;
	int k;
	
	fp=fopen("rank.txt","r");
	fscanf(fp,"%d",&count);//fopen과 fscanf를 통해 rank.txt에 몇 개의 자료가 있는지를 얻는다.
	R=CreateLinkedList();//새로운 LinkedList를 만드는 함수이다. R은 전역변수로 설정되어 있다.
	while(i<count)
	{
		fscanf(fp,"%s %d",str,&sco);//count만큼 개수를 받는다.(즉 모든 자료를 받는다.)
		InsertNode(R,sco,str);//score와 name에 해당하는 자료를 InsertNode 함수를 통해 LinkedList인 R에 넣는다.
		i++;
	}
	
	fclose(fp);//파일을 닫는다.
}

LinkedList *CreateLinkedList()
{
	LinkedList *Rank;//새로운 LinkedList를 만들자.
	Rank=(LinkedList *)malloc(sizeof(LinkedList));//선언
	Rank->head=NULL;//비어있으므로 head는 NULL을 가리킨다.
	Rank->length=0;//비어있으므로 length는 0이다.

	return Rank;//새로 만든 LinkedList를 return한다.
}

void InsertNode(LinkedList *Rank, int sco, char *str)//새로운 노드를 LinkedList에 삽입하는 함수.
{
	Node *newNode;//삽입할 newNode
	Node *current;
	Node *prev;
	Node *temp;
	int i;

	newNode=(Node*)malloc(sizeof(Node));//Node 선언
	newNode->score=sco;//데이터 저장(score)
	strcpy(newNode->name, str);//데이터 저장(name)

	if(Rank->head==NULL)//LinkedList가 비어있었다면
	{
		Rank->head=newNode;//Head는 newNode를 가리키고
		newNode->link=NULL;//newNode는 NULL을 가리키도록
	}
	else//LinkedList가 비어있지 않았다면
	{
		i=0;
		current=Rank->head;
		while(current!=NULL && current->score > newNode->score)//LinkedList에 NewNode가 어디로 들어가야 하는지 판단. score 내림차순으로 넣어야하므로.
		{
			i=1;
			prev=current;//prev는 이전 노드
			current=current->link;
			//prev노드와 current노드에 삽입할 것임.
		}
		if(i==1)//만약 중간에 들어가야 하는 node라면
		{
			//prev와 current 사이에 삽입하도록
			temp=prev;
			newNode->link=temp->link;
			prev->link=newNode;
		}
		else if(i==0)//만약 맨 앞에 들어가야 하는 node라면
		{
			//Rank의 head는 newNode를 가리키고
			Rank->head=newNode;
			//newNode는 Rank가 가리키고 있던 곳을 가리키도록
			newNode->link=current;
		}
	}
	Rank->length++;//1개의 Node를 삽입했으므로 length를 1 증가시킴
}

void PrintList(FILE *fp, LinkedList *R)//LinkedList에 있는 Node를 Print하는 함수
{
	Node *current=R->head;
	fprintf(fp, "%d\n",R->length);//node가 몇 개인지를 알아내자.
	while(current)
	{
		printw("%s %d\n", current->name, current->score);//print하자.
		//fprintf(fp, "%s %d\n", current->name, current->score);//

		current=current->link;//LinkedList를 따라서 한 노드씩 건너가자.
	}
}

void rank(){
	clear();//빈 공간으로
	int input, x, y, pageturn;
	int k;
	char name[NAMELEN];
	//ppt와 같게 print
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	printw("4. list ranks by a score(X to Y)\n");
	printw("5. delete All ranks\n");

	//입력을 받자..
	input=wgetch(stdscr);
	x=0, y=0;
	switch(input)
	{
		case(49)://입력이 만약 1이었다면(아스키 넘버 고려)
			printw("X : ");
			echo();
			//x~y까지 출력할 것이므로, 이를 입력받는다.
			//echo를 통해서 어떤 것을 입력받는지 보이도록.
			scanw("%d", &x);
			printw("Y : ");
			scanw("%d", &y);
			//x~y까지 출력하는 함수 호출.
			fromXtoY(x, y);
			noecho();
			break;
		case(50)://입력이 만약 2였다면
			//이름을 통해서 검색
			printw("input the name: ");
			echo();
			//검색할 이름을 입력받는다.
			scanw("%s",name);
			//이름을 통해 검색하기 위해서 이를 수행하는 함수 호출
			searchname(name);
			noecho();
			break;
		case(51)://입력이 만약 3이었다면
			printw("input the rank: ");
			echo();
			//지울 랭크 입력받기
			scanw("%d",&k);
			//해당 랭크를 지우기 위한 함수를 호출
			deleteRank(k);
			noecho();
			break;
		case(52):
			printw("input the score X: ");
			echo();
			scanw("%d",&x);
			printw("input the score Y: ");
			scanw("%d",&y);
			searchscore(x,y);
			noecho();
			break;
		case(53):
			deleteAll();
			printw("All the rank is deleted");
	}
	pageturn=0;
	//입력을 받을 때 까지 기다린다.
	pageturn=wgetch(stdscr);
	//만약 입력을 받았다면 바깥으로 나가기
	if(pageturn!=0)
		return;
	
}
void fromXtoY(int x, int y)//x~y를 출력하는 함수.
{
	int count=1;
	Node *current=R->head;
	
	if(x==0)//x입력이 없다면 맨 처음으로
		x=1;
	if(y==0)//y입력이 없다면 맨 마지막으로
		y=R->length;
	//printw("%d %d",x,y);
	printw("	name	| score\n");
	printw("------------------------\n");
	if(x>y)
	{
		//x>y라면 잘못된 입력
		printw("search failure: no rank in the list");
		return;
	}
	while(current)
	{
		if(x<=count && count<=y)//x~y에 해당한다면
		{
			//출력하자.(if else는 길이 조절을 위해서.)
			if(strlen(current->name)<8)
				printw(" %s\t\t| %d\n", current->name, current->score);
			else
				printw(" %s\t| %d\n", current->name, current->score);
		}
		current=current->link;//한단계씩 건너가자.
		count++;
	}

}
void writeRankFile(){//새로운 Rank를 파일에 쓰는 함수.
	FILE* fp, fp1;
	//int k=3;
	Node* current=R->head;
	fp=fopen("rank.txt","w");
	fprintf(fp,"%d\n",(int)R->length);//몇 개인지를 파일에 쓰자.
	while(current)
	{
		fprintf(fp,"%s %d\n",current->name, current->score);
		current=current->link;
		//한 단계씩 노드를 건너가며, 모든 데이터들을 파일에 쓴다.
	}
	fclose(fp);//파일을 닫는다.
}

void newRank(int score){//새로운 rank를 LinkedList에 넣자.
	char n[NAMELEN];
	clear();//화면 비우기
	echo();
	printw("your name: ");//이름 입력받기.
	scanw("%s",n);
	noecho();
	if(strcmp(" ",n)==32)//입력하지 않았다면 넣지 말자.
		return;
	
	InsertNode(R,score,n);//새로운 rank를 LinkedList에 삽입
	writeRankFile();//파일에도 업데이트해주자.
	return;
	//R=NULL;
}

void searchname(char* str)//ranking에서 이름 찾기
{
	int i=0;
	Node* current=R->head;
	printw("	name	| score\n");
	printw("------------------------\n");
	while(current)
	{
		if(strcmp(str,current->name)==0)//찾는 이름과 비교해서 같다면
		{
			i=1;
			//print하자
			if(strlen(str)<8)
				printw(" %s\t\t| %d\n",str,current->score);
			else
				printw(" %s\t| %d\n",str,current->score);
		}
		current=current->link;
	}
	//만약 찾는 이름이 없다면 failuer print
	if(i==0)
		printw("\nsearch failure: no name in the list");
}

void deleteRank(int n)//rank를 입력받고 해당 rank를 지우자.
{
	FILE *fp;
	Node* prev;
	Node* current=R->head;
	int k=1;
	int i=0;
	while(current)
	{
		//입력받은 rank가 데이터 개수보다 크다면 failure.
		if(n > R->length)
		{
			i=1;
			printw("search failure: the rank not in the list\n");
			break;
		}
		//입력받은 rank가 1이라면 맨 앞 node 지우기
		if(n==1)
		{
			R->head=current->link;
			current->link=NULL;
			free(current);
			printw("result: the rank deleted\n");
			R->length--;//개수 1개 마이너스
			break;
		}
		//입력받은 rank가 맨 앞이 아니라면 그 rank 지우기
		else if(n==k)
		{
			prev->link=current->link;
			current->link=NULL;
			free(current);
			printw("result: the rank deleted\n");
			R->length--;
			break;
		}
		prev=current;
		current=current->link;
		k++;
	}
	//만약 rank를 지웠다면
	if(i==0)
	{
		//지운 rank를 rank.txt에 다시 업데이트해주기
		fp=fopen("rank.txt","w");
		current=R->head;
		fprintf(fp,"%d\n",R->length);//길이 써주고
		while(current)
		{
			//모든 데이터 써주기
			fprintf(fp,"%s %d\n",current->name, current->score);
			current=current->link;
		}
		fclose(fp);
	}
}

void searchscore(int x, int y)
{
	int i=0, sco;
	Node* current=R->head;
	printw("	name	| score\n");
	printw("------------------------\n");
	while(current)
	{
		sco=current->score;
		if(x<=sco && sco<=y)
		{
			i=1;
			if(strlen(current->name)<8)
				printw(" %s\t\t| %d\n",current->name,sco);
			else
				printw(" %s\t| %d\n",current->name,sco);
		}
		current=current->link;
	}
	if(i==0)
		printw("\nsearch failure: no score in the list");

}

void deleteAll()
{
	int k=R->length;
	while(k--)
	{
		deleteRank(1);
	}
}

void NewRecord(int sco)
{
	int k, pageturn=0;
	init_pair(1,COLOR_YELLOW,COLOR_BLACK);
	init_pair(2,COLOR_CYAN,COLOR_BLACK);
	clear();

	if(R->head==NULL)
		k=0;
	else
		k=R->head->score;

	if(k < sco)
	{
		move(1,WIDTH/2-4);
		attron(COLOR_PAIR(2));
		printw("Congratulaiton!\n");
		attroff(COLOR_PAIR(2));
		printw("You set a ");
		attron(COLOR_PAIR(1));
		printw("New Record!");
		attroff(COLOR_PAIR(1));
		pageturn=wgetch(stdscr);
		if(pageturn!=0)
			return;
	}

}
void DrawRecommend(int y, int x, int blockID,int blockRotate){
	int i, j, input;
	//printf("%d %d %d %d ",y,x,blockID,blockRotate);
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(block[blockID][blockRotate][i][j]==1)
			{
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("R");
				attroff(A_REVERSE);
				refresh();
			}
	move(HEIGHT,WIDTH+1);
	//input=wgetch(stdscr);
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int level=root->lv;
	int sco=root->score;
	RecNode *N, *M;
	int blockRot=0, bX=0, bY=0, touched=0,touch=0;;
	int i, k=0, bR, nb, y, x, h=0, w=0, he;
	int rX,rY,rR,rS,rl;
	int a, b, c; 
	int num=0, dl=0;

	if(level==BLOCK_NUM+1)
	{
        if(recS < sco)
        {
            recS=sco;
            M=root;
            for(i=0;i<BLOCK_NUM-1;i++)
            {
                M=M->parent;
            }
            recX=M->recBlockX;
            recY=M->recBlockY;
            recR=M->recBlockRotate;
        }
		return sco;
	}
	nb=nextBlock[level-1];


	switch(nb)
	{
		case 0 : bR=2; break;
		case 1 : bR=4; break;
		case 2 : bR=4; break;
		case 3 : bR=4; break;
		case 4 : bR=1; break;
		case 5 : bR=2; break;
		case 6 : bR=2; break;
	}
	if(level<BLOCK_NUM)
	{
		switch(nextBlock[level])
		{
			case 0 : k=17; break;
			case 1 : k=34; break;
			case 2 : k=34; break;
			case 3 : k=34; break;
			case 4 : k=9; break;
			case 5 : k=17; break;
			case 6 : k=17; break;
		}
	}
	else
		k=1;

	for(blockRot=0;blockRot<bR;blockRot++)
	{
		bY=1, bX=-1, y=0, x=0;
		if(nb==0&&blockRot==0)
		{	bY=3; bX=0; }
		if(nb==3)
			if(blockRot==3)
				bX=-1;
			else
				bX=0;
		if(nb==1 || nb==2)
			if(blockRot==1)
				bX=-2;
		while(1)
		{
			he=0; y=0; x=0; touched=0; dl=0;
			if(CheckToMove(root->recField,nb,blockRot,bY+he,bX)==0)
			{
				break;
			}
			for(he=0;he<HEIGHT;he++)
			{
				i=CheckToMove(root->recField,nb,blockRot,bY+he,bX);
				if(i==0)
					break;
			}
			
			level=level+1;
            NodeNum++;
			N=CreateNode(level,sco,bY+he-1,bX,blockRot,k);
			InsertToTree(root,N);
			copyField(root->recField,N->recField);
			touched=AddBlockToField(N->recField,nb,blockRot,bY+he-1,bX);
			sco=sco+touched;
			dl=DeleteLine(N->recField);
			sco=sco+dl;
			N->score=sco;
			
			recommend(N);

			bX++;
			sco=sco-touched-dl;
			level=level-1;
			touched=0;

		}
	}
	if(root->lv==1)
	{
		//findmax(root);
		DrawRecommend(recY, recX, nb, recR);
	}
	return max;
}
void recommended(RecNode* root)
{
	recommend(root);
}
/*
void findmax(RecNode* root)
{
	int k[BLOCK_NUM];
	int i;
	int a=0,b=0,c=0,temp=0;
	for(i=0;i<BLOCK_NUM;i++)
	{
		switch(nextBlock[i])
		{
			case 0 : k[i]=17; break; 
			case 1 : k[i]=34; break;
			case 2 : k[i]=34; break;
			case 3 : k[i]=34; break;
			case 4 : k[i]=9; break;
			case 5 : k[i]=17; break;
			case 6 : k[i]=17; break;
		}
	}

	for(a=0;a<k[0];a++)
    {
        if(root->child[a]==NULL)
            continue;
		for(b=0;b<k[1];b++)
        {
            if(root->child[a]->child[b]==NULL)
                continue;
			for(c=0;c<k[2];c++)
			{

				if(root->child[a]->child[b]->child[c]==NULL)
					continue;
				temp=root->child[a]->child[b]->child[c]->score;
				if(temp>recS)
				{
					recX=root->child[a]->recBlockX;
					recY=root->child[a]->recBlockY;
					recR=root->child[a]->recBlockRotate;
					recS=temp;
				}
			}

        }
    }
}*/
void copyField(char f[HEIGHT][WIDTH],char g[HEIGHT][WIDTH])
{
	int i,j;
	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			g[i][j]=f[i][j];
}


void recommendedPlay(){
	// user code
	int command;
	int i=0;
    time_t start, stop;
    //double duration;
	clear();
	InitTetris();
	act.sa_handler= rp;
	sigaction(SIGALRM,&act,&oact);
    start=time(NULL);
	do{
		if(timed_out==0)
		{
			alarm(1);
			timed_out=1;
		}
		command=GetCommand();
		if(ProcessCommand(command)==QUIT)
		{
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Godd-bye!!");
			refresh();
			getch();
            count=0;
            stop=time(NULL);
            duration=(double)difftime(stop,start);
            //printf("The total time : %lf seconds ",duration);
			printeff();
            return;
		}
	}while(!gameOver);
    stop=time(NULL);
    duration=(double)difftime(stop,start);
    count=0;
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
    printeff();
	refresh();
	getch();
}

void rp(int sig)
{
	int k;
	RecNode* root;

	switch(nextBlock[1])
	{
		case 0 : k=17; break;
		case 1 : k=34; break;
		case 2 : k=34; break;
		case 3 : k=34; break;
		case 4 : k=9; break;
		case 5 : k=17; break;
		case 6 : k=17; break;
	}
    if(count==0)
    {
        root=CreateNode(1,0,0,0,0,k);
        copyField(field,root->recField);
        recommended(root);
        score+=AddBlockToField(field,nextBlock[0],recR,recY,recX);
        score+=DeleteLine(field);
        recX=0; recY=0; recS=0; recR=0;
        count++;
    }
    DrawField();
    nextBlock[0]=nextBlock[1];
	nextBlock[1]=nextBlock[2];
	nextBlock[2]=rand()%7;
	DrawNextBlock(nextBlock);
	PrintScore(score);
    DrawBlock(-1,WIDTH/2-2,nextBlock[0],0,' ');
    root=CreateNode(1,0,0,0,0,k);
    copyField(field,root->recField);
    recommended(root);
    if(recY<=0)
        gameOver=1;
    score+=AddBlockToField(field,nextBlock[0],recR,recY,recX);
    score+=DeleteLine(field);
    recX=0; recY=0; recS=0; recR=0;
	timed_out=0;

}

void printeff()
{
    clear();
    printw("The score is : %d points \n",score);
    printw("The time is : %lf seconds\n",duration);
    printw("The space is : %d bytes \n",NodeNum*sizeof(RecNode*));
    printw("Time Efficiecy : %lf\n",(double)score/duration);
    printw("Space Efficiency : %lf\n",((double)score/(double)(NodeNum*sizeof(RecNode*))));
    NodeNum=0;
    getch();
    return;
}
