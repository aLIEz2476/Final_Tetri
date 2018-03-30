#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // sleep함수셔틀
#include <string.h> // memset(), memcpy() 등을 비롯한 문자열 관련 헤더
#include <conio.h> // _getch(), _kbhit()을 사용하기 위한 헤더
#include <time.h> // 다음 블록을 랜덤으로 지정하기 위한 시간 헤더

#define BOARD_X 12 // 가로12
#define BOARD_Y 22 // 세로22

char Board[BOARD_Y][BOARD_X]; // 전역 변수로 테트리스 게임판 생성
char Board_Buffer[BOARD_Y][BOARD_X]; // 게임판 버퍼 생성(실제로 보여주는 부분)
enum { Block_I, Block_O, Block_S, Block_Z, Block_J, Block_L, Block_T, Block_MAX }; // 열거형을 이용하여 각 블록 모양의 플래그 지정(총 7가지)
enum { Block_Angle_0, Block_Angle_90, Block_Angle_180, Block_Angle_270, Block_Angle_360 }; // 90도마다 바뀌는 블록의 플래그 지정
// 각도에 360을 넣는 이유는 키가 한 번 더 눌러졌을 때 0으로 돌아가기 위한 플래그
// 블록에 MAX를 넣는 이유는 단순한 배열만들기 용도


/*블록의 중심점

4 x 4 배열을 이용한다고 가정했을 때, 4 x 4에서 [3][0]에 해당하는 부분을 기준으로 돌리면 된다.
그에 따라 배열 선언도 [3][0]을 기준으로 돌리기로 한다.

*/

#define Block_W 4
#define Block_H 4
char Blocks[Block_MAX][Block_Angle_360][Block_H][Block_W] = {

	// I
	{ { { 0,1,0,0 },{ 0,1,0,0 },{ 0,1,0,0 },{ 0,1,0,0 } },{ { 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },{ { 0,0,1,0 },{ 0,0,1,0 },{ 0,0,1,0 },{ 0,0,1,0 } },{ { 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 } } },
	// O
	{ { { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } } },
	// S
	{ { { 0,0,0,0 },{ 0,1,1,0 },{ 1,1,0,0 },{ 0,0,0,0 } },{ { 0,1,0,0 },{ 0,1,1,0 },{ 0,0,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 1,1,0,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,0,0 },{ 0,1,1,0 },{ 0,0,1,0 } } },
	// Z
	{ { { 0,0,0,0 },{ 1,1,0,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,0,1,0 },{ 0,1,1,0 },{ 0,1,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,0,1,1 },{ 0,0,0,0 } },{ { 0,0,1,0 },{ 0,1,1,0 },{ 0,1,0,0 },{ 0,0,0,0 } } },
	// J
	{ { { 0,0,1,0 },{ 0,0,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 1,1,1,0 },{ 0,0,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,0,0 },{ 0,1,0,0 } },{ { 0,0,0,0 },{ 0,1,0,0},{ 0,1,1,1 },{ 0,0,0,0 } } },
	// L
	{ { { 0,1,0,0 },{ 0,1,0,0 },{ 0,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,0,1,0 },{ 1,1,1,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,0,1,0 },{ 0,0,1,0 } },{ { 0,0,0,0 },{ 0,1,1,1 },{ 0,1,0,0 },{ 0,0,0,0 } } },
	// T
	{ { { 0,0,0,0 },{ 1,1,1,0 },{ 0,1,0,0 },{ 0,0,0,0 } },{ { 0,0,0,0 },{ 0,1,0,0 },{ 0,1,1,0 },{ 0,1,0,0 } },{ { 0,0,0,0 },{ 0,0,1,0 },{ 0,1,1,1 },{ 0,0,0,0 } },{ { 0,0,1,0 },{ 0,1,1,0 },{ 0,0,1,0 },{ 0,0,0,0 } } }

}; // 4 x 4배열을 기반으로 한 블록 데이터 생성(7가지, 4개 각도)

int blockType = 0, blockAngle = 0;
int blockX = 5, blockY = 0;
// 블록이 나타날 위치지정 변수와 종류/각도 변수 생성

int score = 0;


void display()
{
	memcpy_s(Board_Buffer, sizeof(char)*BOARD_Y*BOARD_X, Board, sizeof(char)*BOARD_Y*BOARD_X);
	// memcpy를 사용하여 게임판에 있는 내용을 복사 


	for (int i = 0; i < Block_H; i++)
		for (int j = 0; j < Block_W; j++)
			Board_Buffer[blockY + i][blockX + j] |= Blocks[blockType][blockAngle][i][j];
	// 출력할 블록 만들기

	system("cls"); // 시스템 cls명령을 이용하여 매초마다 화면 갱신
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
			printf(Board_Buffer[i][j] ? "□" : "　");
		printf("\n");
	} // 버퍼 테두리 채우기
} // display 출력 함수 생성

bool isHit(int _blockX, int _blockY, int _blockType, int _blockAngle)
{
	for (int i = 0; i < Block_H; i++)
	{
		for (int j = 0; j < Block_W; j++)
		{
			if (Blocks[_blockType][_blockAngle][i][j] && Board[_blockY + i][_blockX + j])
				return true;
		}
	}
	return false;
}
// 접촉 감지 함수

void resetBlock()
{
	srand(time(NULL));
	blockX = 5; blockY = 0;
	blockType = rand() % Block_MAX;
	blockAngle = rand() % Block_Angle_360;
}

int main()
{
	memset(Board, 0, sizeof(char)*BOARD_Y*BOARD_X); //memset을 활용하여 게임판 초기화

	for (int i = 0; i < BOARD_Y; i++)
	{
		Board[i][0] = Board[i][BOARD_X - 1] = 1;
	}
	for (int i = 0; i < BOARD_X; i++)
		Board[BOARD_Y - 1][i] = 1;
	// 게임판 테두리 채우기

	resetBlock();

	int keyVal = _getch(); // 방향키 감별 임시변수
	time_t t = time(NULL); // 갱신용 타이머 변수

	while (1)
	{
		/*키 입력 인식 부분*/

		if (_kbhit())
		{
			switch (_getch())
			{

			case 'a':
				if (!isHit(blockX - 1, blockY, blockType, blockAngle))
					blockX--;
				break;
				// ←

			case 'd':
				if (!isHit(blockX + 1, blockY, blockType, blockAngle))
					blockX++;
				break;
				// →

			case 's':
				if (!isHit(blockX, blockY + 1, blockType, blockAngle))
					blockY++;
				break;
				// ↓

			case 0x20:
				if (!isHit(blockX, blockY + 1, blockType, (blockAngle + 1) % Block_Angle_360))
					blockAngle = (blockAngle + 1) % Block_Angle_360;
				break;
				// 스패이스키

			}
			display();
		}
		// kbhit()을 이용하여 키 입력 인식함.
		// switch()문으로 키 필터링 및 키에 따른 알맞은 동작 지시

		if (t != NULL)
		{
			t = time(NULL) / 2;
			if (isHit(blockX, blockY + 1, blockType, blockAngle))
			{
				for (int i = 0; i < Block_H; i++)
				{
					for (int j = 0; j < Block_W; j++)
					{
						Board[blockY + i][blockX + j] |= Blocks[blockType][blockAngle][i][j];
					}
				}

				for (int i = 0; i < BOARD_Y - 1; i++)
				{
					bool isLineFull = true; // 1줄 채우기 감지 변수

					for (int j = 1; j < BOARD_X - 1; j++)
					{
						if (!Board[i][j])
							isLineFull = false;
					} // 1줄 채웠는지 감지

					if (isLineFull)
					{
						for (int j = i; 0 < j; j--)
						{
							memcpy_s(Board[j], BOARD_X, Board[j - 1], BOARD_X);
						} // 블록 지우기 및 점수 추가
						score++;
					}
				}
				resetBlock();
			}

			else
			{
				Sleep(200);
				blockY++;// 매초마다 한 칸씩 블록 떨궈주는 역할
			}
			printf("\n%d\n", score);
			if (score >= 100)
			{
				Sleep(3000);
				printf("\n\nㅊㅋㅊㅋ\n%d\n", score);
				break;
			}
			else
			{
				for (int i = 1; i < BOARD_X - 1; i++)
				{
					if (Board[0][i])
					{
						printf("You Game Over~ Ha~Ha~\n");
						Sleep(3000);
						break;
					}
				}
			}
			display();
		}
	}
	
}