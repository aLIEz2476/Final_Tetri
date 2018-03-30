#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // sleep�Լ���Ʋ
#include <string.h> // memset(), memcpy() ���� ����� ���ڿ� ���� ���
#include <conio.h> // _getch(), _kbhit()�� ����ϱ� ���� ���
#include <time.h> // ���� ����� �������� �����ϱ� ���� �ð� ���

#define BOARD_X 12 // ����12
#define BOARD_Y 22 // ����22

char Board[BOARD_Y][BOARD_X]; // ���� ������ ��Ʈ���� ������ ����
char Board_Buffer[BOARD_Y][BOARD_X]; // ������ ���� ����(������ �����ִ� �κ�)
enum { Block_I, Block_O, Block_S, Block_Z, Block_J, Block_L, Block_T, Block_MAX }; // �������� �̿��Ͽ� �� ��� ����� �÷��� ����(�� 7����)
enum { Block_Angle_0, Block_Angle_90, Block_Angle_180, Block_Angle_270, Block_Angle_360 }; // 90������ �ٲ�� ����� �÷��� ����
// ������ 360�� �ִ� ������ Ű�� �� �� �� �������� �� 0���� ���ư��� ���� �÷���
// ��Ͽ� MAX�� �ִ� ������ �ܼ��� �迭����� �뵵


/*����� �߽���

4 x 4 �迭�� �̿��Ѵٰ� �������� ��, 4 x 4���� [3][0]�� �ش��ϴ� �κ��� �������� ������ �ȴ�.
�׿� ���� �迭 ���� [3][0]�� �������� ������� �Ѵ�.

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

}; // 4 x 4�迭�� ������� �� ��� ������ ����(7����, 4�� ����)

int blockType = 0, blockAngle = 0;
int blockX = 5, blockY = 0;
// ����� ��Ÿ�� ��ġ���� ������ ����/���� ���� ����

int score = 0;


void display()
{
	memcpy_s(Board_Buffer, sizeof(char)*BOARD_Y*BOARD_X, Board, sizeof(char)*BOARD_Y*BOARD_X);
	// memcpy�� ����Ͽ� �����ǿ� �ִ� ������ ���� 


	for (int i = 0; i < Block_H; i++)
		for (int j = 0; j < Block_W; j++)
			Board_Buffer[blockY + i][blockX + j] |= Blocks[blockType][blockAngle][i][j];
	// ����� ��� �����

	system("cls"); // �ý��� cls����� �̿��Ͽ� ���ʸ��� ȭ�� ����
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
			printf(Board_Buffer[i][j] ? "��" : "��");
		printf("\n");
	} // ���� �׵θ� ä���
} // display ��� �Լ� ����

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
// ���� ���� �Լ�

void resetBlock()
{
	srand(time(NULL));
	blockX = 5; blockY = 0;
	blockType = rand() % Block_MAX;
	blockAngle = rand() % Block_Angle_360;
}

int main()
{
	memset(Board, 0, sizeof(char)*BOARD_Y*BOARD_X); //memset�� Ȱ���Ͽ� ������ �ʱ�ȭ

	for (int i = 0; i < BOARD_Y; i++)
	{
		Board[i][0] = Board[i][BOARD_X - 1] = 1;
	}
	for (int i = 0; i < BOARD_X; i++)
		Board[BOARD_Y - 1][i] = 1;
	// ������ �׵θ� ä���

	resetBlock();

	int keyVal = _getch(); // ����Ű ���� �ӽú���
	time_t t = time(NULL); // ���ſ� Ÿ�̸� ����

	while (1)
	{
		/*Ű �Է� �ν� �κ�*/

		if (_kbhit())
		{
			switch (_getch())
			{

			case 'a':
				if (!isHit(blockX - 1, blockY, blockType, blockAngle))
					blockX--;
				break;
				// ��

			case 'd':
				if (!isHit(blockX + 1, blockY, blockType, blockAngle))
					blockX++;
				break;
				// ��

			case 's':
				if (!isHit(blockX, blockY + 1, blockType, blockAngle))
					blockY++;
				break;
				// ��

			case 0x20:
				if (!isHit(blockX, blockY + 1, blockType, (blockAngle + 1) % Block_Angle_360))
					blockAngle = (blockAngle + 1) % Block_Angle_360;
				break;
				// �����̽�Ű

			}
			display();
		}
		// kbhit()�� �̿��Ͽ� Ű �Է� �ν���.
		// switch()������ Ű ���͸� �� Ű�� ���� �˸��� ���� ����

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
					bool isLineFull = true; // 1�� ä��� ���� ����

					for (int j = 1; j < BOARD_X - 1; j++)
					{
						if (!Board[i][j])
							isLineFull = false;
					} // 1�� ä������ ����

					if (isLineFull)
					{
						for (int j = i; 0 < j; j--)
						{
							memcpy_s(Board[j], BOARD_X, Board[j - 1], BOARD_X);
						} // ��� ����� �� ���� �߰�
						score++;
					}
				}
				resetBlock();
			}

			else
			{
				Sleep(200);
				blockY++;// ���ʸ��� �� ĭ�� ��� �����ִ� ����
			}
			printf("\n%d\n", score);
			if (score >= 100)
			{
				Sleep(3000);
				printf("\n\n��������\n%d\n", score);
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